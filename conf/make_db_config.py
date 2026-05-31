#!/usr/bin/env python3
"""生成 xms_mysql_init.conf 二进制配置文件"""
import struct
import sys

host = sys.argv[1] if len(sys.argv) > 1 else "localhost"
user = sys.argv[2] if len(sys.argv) > 2 else "root"
password = sys.argv[3] if len(sys.argv) > 3 else "123456"
db_name = sys.argv[4] if len(sys.argv) > 4 else "xms_config"
port = int(sys.argv[5]) if len(sys.argv) > 5 else 3306

fmt = "128s128s128s128si"
packed = struct.pack(fmt,
                     host.encode('utf-8'),
                     user.encode('utf-8'),
                     password.encode('utf-8'),
                     db_name.encode('utf-8'),
                     port)

with open("/etc/xms_mysql_init.conf", "wb") as f:
    f.write(packed)

print(f"DB config written: {host}:{port}, user={user}, db={db_name}")
