

该框架可以分为以下几个层次：

-   APP层：即应用层。SDK提供的代码示例在SDK的代码目录：app\\demo\\src。
-   API层：提供基于SDK开发的通用接口。
-   Platform平台层：提供SOC系统板级支持包，包括如下功能：
    -   芯片和外围器件驱动
    -   操作系统
    -   系统管理

-   Service服务层：提供包含WiFi等应用协议栈。用于上层应用软件进行数据收发等操作。
-   第三方：提供给Service服务层或提供给应用层使用的第三方软件库。



**稳定、可靠的通信能力**

-   支持复杂环境下 TPC、自动速率、弱干扰免疫等可靠性通信算法

**灵活的组网能力**

-   支持 256 节点 Mesh 组网
-   支持标准 20M 带宽组网和 5M/10M 窄带组网

**完善的网络支持**

-   支持 IPv4/IPv6 网络功能
-   支持 DHCPv4/DHCPv6 Client/Server
-   支持 DNS Client 功能
-   支持 mDNS 功能
-   支持 CoAP/MQTT/HTTP/JSON 基础组件

**强大的安全引擎**

-   硬件实现 AES128/256 加解密算法
-   硬件实现 HASH-SHA256、HMAC\_SHA256 算法
-   硬件实现 RSA、ECC 签名校验算法
-   硬件实现真随机数生成，满足 FIPS140-2 随机测试标准
-   硬件支持 TLS/DTLS 加速
-   内部集成 EFUSE，支持安全存储、安全启动、安全升级
-   内部集成 MPU 特性，支持内存隔离特性

**开放的操作系统**

-   丰富的低功耗、小内存、高稳定性、高实时性机制
-   灵活的协议支撑和扩展能力
-   二次开发接口
-   多层级开发接口：操作系统适配接口和系统诊断接口、 链路层接口、网络层接口



W800的SDK软件包根目录结构所在位置device\\winnermicro\\neptune\\sdk\_liteos，如下图所示：

```
device/winnermicro/neptune/sdk_liteos
├── app                 # 应用层代码（其中包含demo程序为参考示例）。
├── boot                # Flash bootloader代码。
├── build               # SDK构建所需的库文件、链接文件、配置文件。
├── BUILD.gn            # GN构建脚本
├── build_patch.sh      # 用于解压uboot开源源码包和打patch。
├── build.sh            # 启动编译脚本，同时支持“sh build.sh menuconfig”进行客制化配置。
├── components          # SDK平台相关的组件
├── config              # SDK系统配置文件。
├── config.gni          # 支持HarmonyOS配置文件。
├── factory.mk          # 厂测版本编译脚本。
├── hm_build.sh         # 适配HarmonyOS构建脚本。
├── include             # API头文件存放目录。
├── license             # SDK开源license声明
├── Makefile            # 支持make编译，使用“make”或“make all”启动编译。
├── non_factory.mk      # 非厂测版本编译脚本。
├── platform            # SDK平台相关的文件（包括：内核镜像、驱动模块等）
├── SConstruct          # SCons编译脚本。
├── third_party         # 开源第三方软件目录。
└── tools               # SDK提供的Linux系统和Windows系统上使用的工具（包括：NV制作工具、签名工具、Menuconfig等）。
```



hmf/vendor/winnermicro

