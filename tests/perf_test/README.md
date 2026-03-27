# 性能测试 (perf_test)

## 概述

本测试套件用于对 OrbbecSDK 进行长时间运行的性能评估，覆盖丢帧检测和高负载场景。

## 测试用例

### TC_PERF_01 — 丢帧检测 (baseline)

对 depth / color / IR 三路数据流进行长时间采集，统计：

- **帧索引连续性**：检查 SDK frame index 和硬件 metadata frame number (`OB_FRAME_METADATA_TYPE_FRAME_NUMBER`) 是否逐帧递增，不连续即判定为丢帧。
- **时间戳连续性**：分别检查硬件时间戳、系统时间戳、全局时间戳相邻帧间隔是否在 `1/fps ± 50%` 范围内，超出判定为时间戳异常。
- **CPU / 内存占用**：后台线程每秒采样一次进程级 CPU 和内存（RSS）。
- **CSV 输出**：每路流生成 `{prefix}_{stream}_frames.csv`，资源数据生成 `{prefix}_resource.csv`。

### TC_PERF_02 — 高 CPU 负载下丢帧检测

在 TC_PERF_01 基础上，启动多线程 CPU 密集型运算（默认线程数 = 逻辑 CPU 核心数），观测高 CPU 占用对帧传输的影响。

### TC_PERF_03 — 高 IO 负载下丢帧检测

在 TC_PERF_01 基础上，启动多线程大文件读写（默认 2 线程，每轮写入 10 MB），观测高磁盘 IO 对帧传输的影响。

### TC_TS_01 — 多线程并发 start/stop Pipeline

多个线程对同一 `Pipeline` 实例反复调用 `start()` / `stop()`，验证 SDK 在最常见的误用场景下不会崩溃或死锁。`ob::Error` 异常视为可接受（SDK 拒绝非法并发操作属于正当行为）。

### TC_TS_02 — 取流与属性读写并发

一个线程持续调用 `waitForFrameset()` 拉取深度流数据，另一个线程循环对设备可读写 Int 属性执行 `getIntProperty()` / `setIntProperty()`，验证流传输与设备控制的并发安全性。

### TC_TS_03 — 多线程并发创建/销毁 Pipeline

多个线程各自独立创建 `Pipeline`、启动取流、拉取少量帧后销毁，验证 USB/设备句柄等底层资源在并发竞争下不会崩溃。

## 环境变量

| 变量名 | 默认值 | 说明 |
|--------|--------|------|
| `PERF_DURATION_SECONDS` | `60` | 每个测试用例的采集时长（秒） |
| `PERF_CPU_THREADS` | 逻辑核心数 | TC_PERF_02 CPU 压力线程数 |
| `PERF_IO_THREADS` | `2` | TC_PERF_03 IO 压力线程数 |
| `TS_DURATION_SECONDS` | `10` | 线程安全测试每个用例的运行时长（秒） |
| `TS_THREAD_COUNT` | `4` | 线程安全测试的并发线程数 |

## 运行方式

```bash
# 默认 60 秒
./ob_perf_test

# 自定义时长
set PERF_DURATION_SECONDS=300
./ob_perf_test

# 只运行 baseline 丢帧检测
./ob_perf_test --gtest_filter="TC_PERF_Stream.TC_PERF_01*"

# 只运行线程安全测试
./ob_perf_test --gtest_filter="ThreadSafetyTest.*"

# 自定义线程安全测试参数
set TS_DURATION_SECONDS=30
set TS_THREAD_COUNT=8
./ob_perf_test --gtest_filter="ThreadSafetyTest.*"
```

## CSV 输出格式

**帧数据 (`{prefix}_{stream}_frames.csv`)**：

```
SdkIndex,MetaFrameNumber,HwTimestamp_us,SysTimestamp_us,GlobalTimestamp_us,FPS
```

**资源数据 (`{prefix}_resource.csv`)**：

```
ElapsedSec,CPU_Percent,Memory_MB
```
