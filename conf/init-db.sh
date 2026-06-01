#!/bin/bash
# ======================================================
# MySQL 初始化脚本
# 创建 XMS 所需的三个数据库
# 此脚本由 Docker MySQL 镜像在首次启动时自动执行
# ======================================================
set -e

echo ">>> Initializing XMS databases..."

# 创建 xms_config 数据库
mysql -u"${MYSQL_ROOT_USER:-root}" -p"${MYSQL_ROOT_PASSWORD}" <<-EOSQL
    CREATE DATABASE IF NOT EXISTS xms_config CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
    CREATE DATABASE IF NOT EXISTS xms_auth CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
    CREATE DATABASE IF NOT EXISTS xms_log CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
    FLUSH PRIVILEGES;
EOSQL

echo ">>> Databases created: xms_config, xms_auth, xms_log"
