#!/bin/bash
#===============================================================================
# xms 微服务云盘项目 - Ubuntu 一键构建脚本
# 按依赖顺序编译所有模块并安装
#===============================================================================

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info()  { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# 项目根目录
PROJ_DIR="$(cd "$(dirname "$0")" && pwd)"

# 记录失败模块
FAILED=""

build_module() {
    local dir="$1"
    local target="$2"
    local extra="$3"

    echo ""
    log_info "===== 开始构建: $dir ($target) ====="
    cd "$PROJ_DIR/$dir"

    # 执行额外命令（如 proto 生成）
    if [ -n "$extra" ]; then
        log_info "执行: $extra"
        eval "$extra" || {
            log_error "$dir: $extra 失败"
            FAILED="$FAILED $dir"
            cd "$PROJ_DIR"
            return 1
        }
    fi

    make clean 2>/dev/null || true
    make || {
        log_error "$dir: 编译失败"
        FAILED="$FAILED $dir"
        cd "$PROJ_DIR"
        return 1
    }
    sudo make install || {
        log_error "$dir: 安装失败"
        FAILED="$FAILED $dir"
        cd "$PROJ_DIR"
        return 1
    }
    log_info "===== $dir 构建完成 ====="
    cd "$PROJ_DIR"
}

#===============================================================================
# 0. 前置检查
#===============================================================================
log_info "检查依赖工具..."
for cmd in g++ make protoc sudo; do
    if ! command -v $cmd &>/dev/null; then
        log_error "未找到 $cmd，请先安装"
        exit 1
    fi
done

#===============================================================================
# 1. 基础库（构建顺序重要！）
#===============================================================================
build_module "xplatform"     "libxcom.so"
build_module "LXMysql"       "libLXMysql.so"
build_module "xdisk_pb"      "libxdisk_pb.so"    "make proto"

#===============================================================================
# 2. 客户端库（被服务端依赖）
#===============================================================================
build_module "register_client" "libregister_client.so"
build_module "config_client"   "libconfig_client.so"

#===============================================================================
# 3. 服务端可执行程序
#===============================================================================
build_module "register_server"     "register_server"
build_module "config_server"       "config_server"
build_module "xauth"               "xauth"
build_module "xlog"                "xlog"
build_module "xms_gateway"         "xms_gateway"
build_module "xms_dir_service"     "xms_dir_service"
build_module "xms_upload_service"  "xms_upload"
build_module "xms_download_service" "xms_download"
build_module "xms_add_user"        "xms_add_user"

#===============================================================================
# 结果汇总
#===============================================================================
echo ""
echo "============================================"
if [ -z "$FAILED" ]; then
    log_info "所有模块构建成功！"
else
    log_error "以下模块构建失败:$FAILED"
    exit 1
fi
echo "============================================"
