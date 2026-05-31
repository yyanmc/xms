#!/bin/bash
#===============================================================================
# XMS 微服务云盘系统 - 一键启停脚本
# 支持 start / stop / restart / status 命令
#===============================================================================

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info()  { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

MYSQL_PASS="123456"
REGISTER_IP="127.0.0.1"
REGISTER_PORT=20018
LOG_DIR="/tmp/xms"

SUDO_PASS="qwe123"

# 自动检测服务器IP用于服务注册（使客户端能通过此IP直连上传/下载等服务）
SERVER_IP=$(hostname -I 2>/dev/null | awk '{print $1}')
if [ -z "$SERVER_IP" ]; then
    SERVER_IP="127.0.0.1"
fi
log_info "服务器IP: $SERVER_IP"

# 服务定义: name, start_script, port, db_name
SERVICES=(
    "register_server:start_register_server:20018:"
    "config_server:start_config_server:20019:xms_config"
    "xlog:start_xlog:20030:xms_log"
    "xms_dir_service:start_xms_dir_service:20300:"
    "xms_upload:start_xms_upload:20100:"
    "xms_download:start_xms_download:20200:"
    "xms_gateway:start_xms_gateway:20010:"
)

# 鉴权服务单独处理（DB 配置与其他服务不同）
start_xms_auth_svc() {
    init_db_config "xms_auth"
    log_info "启动 xms_auth (端口 20020)..."
    start_xms_auth 20020 "$REGISTER_IP" "$REGISTER_PORT" > "$LOG_DIR/xms_auth.log" 2>&1
    sleep 2
    # 恢复上一个服务的 DB 配置，避免 watchdog 重启时读错
    init_db_config "xms_log"
}

init_db_config() {
    local db_name="$1"
    if [ -z "$db_name" ]; then
        return
    fi
    log_info "设置数据库配置: db=$db_name"
    echo "$SUDO_PASS" | sudo -S python3 /home/yan1/xms/conf/make_db_config.py "localhost" "root" "$MYSQL_PASS" "$db_name" 3306 2>&1 | grep -v "password"
}

start_service() {
    local name="$1"
    local script="$2"
    local port="$3"
    local db_name="$4"

    if [ -n "$db_name" ]; then
        init_db_config "$db_name"
    fi

    log_info "启动 $name (端口 $port)..."
    mkdir -p "$LOG_DIR" 2>/dev/null
    local log_file="$LOG_DIR/${name}.log"
    case "$name" in
        register_server)
            $script "$port" > "$log_file" 2>&1
            ;;
        config_server)
            $script "$port" > "$log_file" 2>&1
            ;;
        xauth)
            $script "$port" "$REGISTER_IP" "$REGISTER_PORT" > "$log_file" 2>&1
            ;;
        xlog)
            $script "$REGISTER_IP" "$REGISTER_PORT" "$port" > "$log_file" 2>&1
            ;;
        xms_dir_service)
            $script "$port" "$REGISTER_IP" "$REGISTER_PORT" > "$log_file" 2>&1
            ;;
        xms_upload)
            $script "$REGISTER_IP" "$REGISTER_PORT" "$port" "$SERVER_IP" > "$log_file" 2>&1
            ;;
        xms_download)
            $script "$REGISTER_IP" "$REGISTER_PORT" "$port" "$SERVER_IP" > "$log_file" 2>&1
            ;;
        xms_gateway)
            $script "$port" "$REGISTER_IP" "$REGISTER_PORT" > "$log_file" 2>&1
            ;;
    esac
    sleep 2
}

stop_service() {
    local name="$1"
    log_info "停止 $name..."
    local watch="watch_${name}"
    killall "$watch" 2>/dev/null || true
    killall "$name" 2>/dev/null || true
    sleep 1
}

status_service() {
    local name="$1"
    if pgrep -x "$name" > /dev/null 2>&1; then
        echo -e "  $name: ${GREEN}运行中${NC}"
    else
        echo -e "  $name: ${RED}未运行${NC}"
    fi
}

case "${1:-start}" in
    start)
        log_info "========== 启动 XMS 所有服务 =========="

        # 检查 MySQL
        if ! mysqladmin ping -u root -p"$MYSQL_PASS" --silent 2>/dev/null; then
            log_error "MySQL 未运行，请先启动 MySQL"
            exit 1
        fi
        log_info "MySQL 连接正常"

        for svc in "${SERVICES[@]}"; do
            IFS=':' read -r name script port db_name <<< "$svc"
            start_service "$name" "$script" "$port" "$db_name"
        done

        # 鉴权服务单独启动（使用独立的 DB 配置）
        start_xms_auth_svc

        log_info "========== 所有服务启动完成 =========="
        echo ""
        log_info "服务列表:"
        echo "  注册中心(register_server):  端口 20018"
        echo "  配置中心(config_server):    端口 20019"
        echo "  鉴权服务(xauth):            端口 20020"
        echo "  日志服务(xlog):             端口 20030"
        echo "  目录服务(xms_dir_service):  端口 20300"
        echo "  上传服务(xms_upload):       端口 20100"
        echo "  下载服务(xms_download):     端口 20200"
        echo "  API网关(xms_gateway):       端口 20010"
        ;;

    stop)
        log_info "========== 停止 XMS 所有服务 =========="
        for ((i=${#SERVICES[@]}-1; i>=0; i--)); do
            IFS=':' read -r name _ _ _ <<< "${SERVICES[i]}"
            stop_service "$name"
        done
        stop_service "xms_auth"
        log_info "所有服务已停止"
        ;;

    restart)
        bash "$0" stop
        sleep 2
        bash "$0" start
        ;;

    status)
        log_info "========== XMS 服务状态 =========="
        for svc in "${SERVICES[@]}"; do
            IFS=':' read -r name _ _ _ <<< "$svc"
            status_service "$name"
        done
        status_service "xms_auth"
        ;;

    *)
        echo "用法: $0 {start|stop|restart|status}"
        exit 1
        ;;
esac
