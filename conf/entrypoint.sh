#!/bin/bash
# ======================================================
# XMS 微服务 - Docker Entrypoint
# 启动前根据环境变量生成 MySQL 配置文件
# ======================================================
set -e

# 如果设置了 DB 环境变量，生成 MySQL 配置文件
if [ -n "$DB_NAME" ]; then
    DB_HOST="${DB_HOST:-mysql}"
    DB_PORT="${DB_PORT:-3306}"
    DB_USER="${DB_USER:-root}"
    DB_PASS="${DB_PASS:-${MYSQL_ROOT_PASSWORD:-root123}}"

    echo ">>> Generating MySQL config: ${DB_HOST}:${DB_PORT} db=${DB_NAME} user=${DB_USER}"
    python3 /usr/local/bin/make_db_config.py \
        "$DB_HOST" "$DB_USER" "$DB_PASS" "$DB_NAME" "$DB_PORT"
fi

# 执行传入的命令（启动服务）
exec "$@"
