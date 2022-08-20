# 你好❤️

> 本项目基于mit-xv6-riscv

<img src="./doc/assets/logo1.png" alt="image-20220411160525296" width=300 />

## 特性

- 支持页缓存&快缓存
- 支持多线程（用户线程&内核线程）
- 高效IO（支持SD卡多块读写）
- Buddy&Slob实现
- 兼容musl标准库
- 内核与用户态公共页表
- 支持多平台bsp开发
- 通用递归的项目构建方式
- 友好的系统调用定义生成

## 项目构建

### 内核构建

在项目根目录下输入:
```shell
$ ./run-qemu.sh # qemu
$ ./run-k210.sh # k210
```

### 文件系统构建
```shell
$ make sdcard # 注意：默认ttyUSB0
```

## 文档

文档分为四个部分：
- 概要文档
- 详细文档
- 测试日志
- 变更日志


## 目录
```
├── bootloader  SBI相关
├── build       编译输出
├── changelog   项目变更日志
├── doc         项目详细文档
├── entry       系统调用表与性能分析表
├── include     头文件目录
├── Makefile    构建文件
├── README      此文件
├── run-gdb.sh  
├── run-k210.sh
├── run-qemu.sh
├── script      编译用脚本
├── src         源文件目录
├── test        测试调试目录，包含测试日志
├── tools       常用工具
├── user        用户程序目录
└── 操作系统内核设计赛文档.pdf
```

## 拓展内容

支持vi编辑器，JavaScript引擎qjs以及c语言解释器xc

## K210八大坑点

1. PUM位不要动（与1.12 SUM位作用相反）
2. S态无外部中断
3. 后2M内存读写会卡死
4. SD卡读写注意中断导致等待超时
5. 需要手动情空bss段
6. PLIC设置S态无效，同2
7. JTAG调试很坑
8. 1.9特权级页错误类型有缺

