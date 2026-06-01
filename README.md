# XMS Cloud Disk

基于 C++17 的分布式网盘系统，采用微服务架构，支持 Docker 一键部署。

## 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        Windows Client (Qt GUI)                   │
│                    config_gui  /  xms_disk_client_gui            │
└──────────────────────────┬──────────────────────────────────────┘
                           │ HTTP/TCP
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                     API Gateway (xms_gateway)                     │
│                       Port: 20010                                 │
│           路由转发 / 负载均衡 / 服务发现代理                       │
└──┬────────┬────────┬────────┬────────┬────────┬────────┬─────────┘
   │        │        │        │        │        │        │
   ▼        ▼        ▼        ▼        ▼        ▼        ▼
┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐
│Auth  │ │Config│ │XLog  │ │Dir   │ │Upload│ │Down- │ │Reg-  │
│      │ │      │ │      │ │      │ │      │ │load  │ │ister │
│20020 │ │20019 │ │20030 │ │20300 │ │20100 │ │20200 │ │20018 │
└──┬───┘ └──┬───┘ └──┬───┘ └──┬───┘ └──┬───┘ └──┬───┘ └──┬───┘
   │        │        │        │        │        │        │
   └────────┴────────┴────────┴────────┴────────┴────────┘
                            │
                            ▼
                    ┌────────────────┐
                    │     MySQL       │
                    │   (5.7)         │
                    │ xms_auth        │
                    │ xms_config      │
                    │ xms_log         │
                    └────────────────┘
```

## 技术栈

| 组件 | 技术 |
|------|------|
| 语言 | C++17 |
| 网络框架 | libevent (事件驱动, 非阻塞 I/O) |
| 序列化 | Protocol Buffers (protobuf) |
| 数据库 | MySQL 5.7 |
| 容器化 | Docker / Docker Compose |
| GUI 客户端 | Qt 5 (Widgets) |
| 构建系统 | GNU Make (Linux), MSBuild (Windows) |
| 密码学 | MD5 + Base64 (自定义 XMD5) |

## 项目结构

```
xms/
├── .dockerignore            # Docker 构建忽略规则
├── .gitignore               # Git 忽略规则
├── Dockerfile               # Docker 多阶段构建
├── docker-compose.yml       # 微服务编排
├── build_all.sh             # Linux 一键构建脚本
├── xmsctl.sh                # 服务管理脚本 (启动/停止/状态)
├── README.md                # 项目文档
│
├── xplatform/               # 核心框架 (libxcom.so)
│   ├── XMsgEvent            # 消息事件处理
│   ├── XThreadPool          # 线程池
│   ├── XService             # 微服务基类
│   ├── XComTask             # TCP 通信任务
│   └── xtools               # 工具函数 (MD5/Base64/AES)
│
├── LXMysql/                 # MySQL 封装库 (libLXMysql.so)
│
├── xdisk_pb/                # 网盘 Protobuf 定义 (libxdisk_pb.so)
│
├── register_client/         # 注册中心客户端 (libregister_client.so)
├── register_server/         # 注册中心服务 (端口:20018)
│
├── config_client/           # 配置中心客户端 (libconfig_client.so)
├── config_server/           # 配置中心服务 (端口:20019)
│
├── xauth/                   # 认证服务 (端口:20020)
│   ├── xauth_server         # 认证服务端
│   └── xauth_client         # 认证客户端库 (libxauth.so)
│
├── xlog/                    # 日志服务 (端口:20030)
│
├── xms_gateway/             # API 网关 (端口:20010)
│   ├── 请求路由与转发
│   ├── 微服务负载均衡 (轮询)
│   └── 外部服务发现
│
├── xms_dir_service/         # 目录服务 (端口:20300)
├── xms_upload_service/      # 上传服务 (端口:20100)
├── xms_download_service/    # 下载服务 (端口:20200)
│
├── xms_add_user/            # 用户管理 CLI 工具
│
├── clients/                 # Windows GUI 客户端
│   ├── config_gui/          # 配置管理 GUI (Qt)
│   └── xms_disk_client_gui/ # 网盘客户端 GUI (Qt)
│
└── conf/                    # 配置文件与脚本
    ├── entrypoint.sh        # Docker 容器入口
    ├── init-db.sh           # 数据库初始化
    └── make_db_config.py    # 数据库配置生成
```

## 快速开始

### 前置要求

- Docker & Docker Compose (推荐)
- 或 Linux 环境 (Ubuntu 22.04+) 直接编译运行
- 或 Windows + Visual Studio 2022 编译客户端

### Docker 一键部署

```bash
# 1. 构建镜像
docker compose build

# 2. 启动所有服务
docker compose up -d

# 3. 查看服务状态
docker compose ps

# 4. 查看日志
docker compose logs -f
```

启动后会自动创建 8 个微服务容器 + 1 个 MySQL 容器，并按依赖顺序启动。

### 默认用户

| 用户名 | 密码 |
|--------|------|
| root | 123456 |

### 服务端口

| 服务 | 端口 | 说明 |
|------|------|------|
| MySQL | 3306 | 数据库 (仅容器内) |
| register_server | 20018 | 注册中心 |
| config_server | 20019 | 配置中心 |
| xauth | 20020 | 认证服务 |
| xlog | 20030 | 日志服务 |
| xms_gateway | 20010 | API 网关 |
| xms_upload_service | 20100 | 上传服务 |
| xms_download_service | 20200 | 下载服务 |
| xms_dir_service | 20300 | 目录服务 (仅容器内) |

### 查看运行状态

```bash
docker compose ps
```

预期输出 (所有服务应为 `Up` 状态):

```
NAME              SERVICE           STATUS          PORTS
xms-mysql         mysql             healthy         3306/tcp
xms-register      register          running          0.0.0.0:20018->20018/tcp
xms-config        config            running
xms-auth          auth              running
xms-xlog          xlog              running
xms-dir           dir               running
xms-upload        upload             running          0.0.0.0:20100->20100/tcp
xms-download      download           running          0.0.0.0:20200->20200/tcp
xms-gateway       gateway           running          0.0.0.0:20010->20010/tcp
```

## Linux 本地编译

适用于 Ubuntu 22.04，需先安装依赖：

```bash
# 安装依赖
sudo apt-get install -y \
    g++ make \
    libprotobuf-dev protobuf-compiler libprotoc-dev \
    libevent-dev libssl-dev \
    libmysqlclient-dev

# 一键构建所有模块
./build_all.sh

# 或使用管理脚本
./xmsctl.sh start       # 启动所有服务
./xmsctl.sh stop        # 停止所有服务
./xmsctl.sh status      # 查看状态
```

## Windows 客户端编译

使用 Visual Studio 2022 打开客户端项目:

1. 安装 Qt 5.15+ 和 Qt Visual Studio Tools
2. 打开 `clients/config_gui/config_gui.sln` - 配置管理工具
3. 打开 `clients/xms_disk_client_gui/xms_disk_client_gui.sln` - 网盘客户端

### 客户端连接配置

1. 修改 Windows 的 `hosts` 文件 (`C:\Windows\System32\drivers\etc\hosts`)，添加:

```
127.0.0.1 xms_register_server
127.0.0.1 xms_gateway_server
```

2. 启动网盘客户端，在登录界面输入默认账号 `root` / `123456`

## 服务依赖关系

```
register_server (20018)            # 先启动：注册中心
    ├── config_server (20019)      # 配置中心 (依赖 register + mysql)
    ├── xauth (20020)              # 认证服务 (依赖 register + mysql)
    ├── xlog (20030)               # 日志服务 (依赖 register + mysql)
    ├── xms_dir_service (20300)    # 目录服务 (依赖 register)
    ├── xms_upload_service (20100) # 上传服务 (依赖 register)
    ├── xms_download_service (20200) # 下载服务 (依赖 register)
    └── xms_gateway (20010)        # API 网关 (依赖 register + auth + dir)
        └── Windows Clients        # GUI 客户端通过网关访问
```

## 微服务通信

服务间通信基于自定义的消息协议 (protobuf):

- **消息格式**: `XMsgHead` + `XMsg` (protobuf 序列化)
- **传输层**: TCP (libevent 非阻塞 I/O)
- **服务发现**: 启动时向注册中心注册，定时心跳保活
- **负载均衡**: API 网关对同一服务的多个实例进行轮询分发

## 存储

- **MySQL 数据库**: 存储用户、配置、日志数据
- **文件存储**: 容器内路径 `/home/yan1/xms/server_root/`，通过 Docker volume `xms-data` 持久化
- **每个用户目录**: `/home/yan1/xms/server_root/{username}/`

## 许可

MIT License
