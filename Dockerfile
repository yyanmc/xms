# ======================================================
# XMS 微服务 - Docker 多阶段构建
# ======================================================

# ---- Stage 1: 基础构建环境 ----
FROM ubuntu:22.04 AS base

ENV DEBIAN_FRONTEND=noninteractive

# 安装构建工具和所有依赖
RUN apt-get update && apt-get install -y \
    g++ make \
    libprotobuf-dev protobuf-compiler libprotoc-dev \
    libevent-dev libssl-dev \
    libmysqlclient-dev \
    python3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

# GCC 11 (Ubuntu 22.04) 默认支持 C++17

# 复制公共库源码
COPY xplatform ./xplatform
COPY LXMysql ./LXMysql
COPY xdisk_pb ./xdisk_pb
COPY register_client ./register_client
COPY config_client ./config_client

# 用系统 protoc 重新生成 .pb.h/.pb.cc（匹配系统 protobuf 版本）
RUN cd xplatform && protoc -I=./ --cpp_out=./ xmsg_com.proto xmsg_type.proto
RUN cd xdisk_pb && protoc -I=./ --cpp_out=./ *.proto

# 按依赖顺序构建公共库
RUN echo ">>> Building xplatform..." && \
    cd xplatform && make && make install && cd /src
RUN echo ">>> Building LXMysql..." && \
    cd LXMysql && make && make install && cd /src
RUN echo ">>> Building xdisk_pb..." && \
    cd xdisk_pb && make && make install && cd /src
RUN echo ">>> Building register_client..." && \
    cd register_client && make && make install && cd /src
RUN echo ">>> Building config_client..." && \
    cd config_client && make && make install && cd /src

# ---- Stage 2: 构建微服务二进制 ----
FROM base AS build

# 复制所有服务源码
COPY register_server ./register_server
COPY config_server ./config_server
COPY xauth ./xauth
COPY xlog ./xlog
COPY xms_gateway ./xms_gateway
COPY xms_dir_service ./xms_dir_service
COPY xms_upload_service ./xms_upload_service
COPY xms_download_service ./xms_download_service

# 构建各个服务
RUN cd register_server && rm -rf register_server && make && cp register_server /usr/bin/
RUN cd config_server && rm -rf config_server && make && cp config_server /usr/bin/
RUN cd xauth && make && make install
RUN cd xlog && rm -rf xlog && make && cp xlog /usr/bin/
RUN cd xms_gateway && rm -rf xms_gateway && make && cp xms_gateway /usr/bin/
RUN cd xms_dir_service && rm -rf xms_dir_service && make && cp xms_dir_service /usr/bin/
RUN cd xms_upload_service && make && cp xms_upload /usr/bin/
RUN cd xms_download_service && make && cp xms_download /usr/bin/

# ---- Stage 3: 运行镜像 ----
FROM ubuntu:22.04 AS runtime

# 安装运行时依赖（最小集合）
RUN apt-get update && apt-get install -y \
    libprotobuf23 \
    libprotoc23 \
    libevent-2.1-7 \
    libevent-openssl-2.1-7 \
    libssl3 \
    libmysqlclient21 \
    python3 \
    && rm -rf /var/lib/apt/lists/* \
    && ldconfig

# 复制构建好的库
COPY --from=build /usr/lib/libxcom.so /usr/lib/
COPY --from=build /usr/lib/libLXMysql.so /usr/lib/
COPY --from=build /usr/lib/libxdisk_pb.so /usr/lib/
COPY --from=build /usr/lib/libregister_client.so /usr/lib/
COPY --from=build /usr/lib/libconfig_client.so /usr/lib/
COPY --from=build /usr/lib/libxauth.so /usr/lib/

# 复制构建好的二进制
COPY --from=build /usr/bin/register_server /usr/bin/
COPY --from=build /usr/bin/config_server /usr/bin/
COPY --from=build /usr/bin/xauth /usr/bin/
COPY --from=build /usr/bin/xlog /usr/bin/
COPY --from=build /usr/bin/xms_gateway /usr/bin/
COPY --from=build /usr/bin/xms_dir_service /usr/bin/
COPY --from=build /usr/bin/xms_upload /usr/bin/
COPY --from=build /usr/bin/xms_download /usr/bin/

# 复制配置文件生成脚本
COPY conf/make_db_config.py /usr/local/bin/make_db_config.py
COPY conf/entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# 创建网盘存储根目录和默认用户目录
RUN mkdir -p /home/yan1/xms/server_root/root/

ENTRYPOINT ["/entrypoint.sh"]
