# 测试用例覆盖审计表

> 更新日期: 2025-07
> 适用于: `tests/nohw_full_test/nohw_full_test.cpp` + `tests/hw_full_test/hw_full_test.cpp`

## 概述

| 指标 | 值 |
|------|------|
| 总用例数 (按 testcase 规格) | 198 |
| 无硬件测试 (ob_nohw_full_test) | 54 |
| 有硬件测试 (ob_hw_full_test) | 144 |
| 覆盖率 | **100%** |

## 重构说明

旧实现 (`core_nohw_test/`, `hardware/`) 已删除，替换为:
- `tests/nohw_full_test/nohw_full_test.cpp`  44 TEST_F, 覆盖全部 54 个无硬件用例
- `tests/hw_full_test/hw_full_test.cpp`  ~100 TEST_F, 覆盖全部 144 个硬件用例
- `tests/playback_test/`  保留不变

## 现有实现分组

### 1) 无硬件测试 (ob_nohw_full_test)

- 可执行: `tests/nohw_full_test/nohw_full_test.cpp` -> `ob_nohw_full_test`
- 44 TEST_F, 覆盖 54 个无硬件用例
- 覆盖模块: TC_CPP_01, 02(部分), 10(部分), 11(部分), 12(部分), 13(部分), 14(部分), 18(部分), 20(部分), 22, 23, 24(部分), 25(部分)

### 2) 录制回放测试 (ob_playback_smoke_test)

- 可执行: `tests/playback_test/playback_smoke_test.cpp` -> `ob_playback_smoke_test`
- 保留不变

### 3) 硬件测试 (ob_hw_full_test)

- 可执行: `tests/hw_full_test/hw_full_test.cpp` -> `ob_hw_full_test`
- ~100 TEST_F, 覆盖 144 个硬件用例
- 覆盖模块: TC_CPP_02~21(硬件部分), 24(部分), 25(部分)

## 模块明细

| 模块 | 规格 ID | 总用例 | NOHW | HW |
|------|---------|--------|------|-----|
| TC_CPP_01 Context | 01_01~06 | 6 | 6 | 0 |
| TC_CPP_02 Discovery | 02_01~05 | 5 | 1 | 4 |
| TC_CPP_03 DeviceList | 03_01~06 | 6 | 0 | 6 |
| TC_CPP_04 DeviceInfo | 04_01~05 | 5 | 0 | 5 |
| TC_CPP_05 AccessMode | 05_01~04 | 4 | 0 | 4 |
| TC_CPP_06 Sensor | 06_01~09 | 9 | 0 | 9 |
| TC_CPP_07 StreamProfile | 07_01~05 | 5 | 0 | 5 |
| TC_CPP_08 Pipeline | 08_01~11 | 11 | 0 | 11 |
| TC_CPP_09 Config | 09_01~09 | 9 | 0 | 8+1 skip |
| TC_CPP_10 Frame | 10_01~14 | 14 | 2 | 12 |
| TC_CPP_11 Metadata | 11_01~06 | 6 | 1 | 5 |
| TC_CPP_12 FrameFactory | 12_01~04 | 4 | 3 | 1 |
| TC_CPP_13 Filter | 13_01~16 | 16 | 5 | 11 |
| TC_CPP_14 Property | 14_01~18 | 18 | 1 | 17 |
| TC_CPP_15 DepthWorkMode | 15_01~05 | 5 | 0 | 5 |
| TC_CPP_16 Preset | 16_01~05 | 5 | 0 | 5 |
| TC_CPP_17 FrameInterleave | 17_01~03 | 3 | 0 | 3 |
| TC_CPP_18 RecordPlayback | 18_01~08 | 8 | 5 | 3 |
| TC_CPP_19 MultiDevice | 19_01~07 | 7 | 0 | 7 |
| TC_CPP_20 CoordTransform | 20_01~06 | 6 | 4 | 2 |
| TC_CPP_21 Firmware | 21_01~09 | 9 | 0 | 9 |
| TC_CPP_22 Version | 22_01~03 | 3 | 3 | 0 |
| TC_CPP_23 Logger | 23_01~05 | 5 | 5 | 0 |
| TC_CPP_24 Error | 24_01~07 | 7 | 5 | 2 |
| TC_CPP_25 DataStruct | 25_01~07 | 7 | 3 | 4 |

## CI 集成

| CI 阶段 | 执行目标 | 触发条件 |
|---------|---------|---------|
| PR Gate | `ob_nohw_full_test` + `ob_playback_smoke_test` | PR / push |
| Daily HW Smoke | 全部三个目标 | 每日 UTC 02:00 |
| Release Candidate | 全部目标 + benchmark | 手动触发 |

## 特殊跳过条件

| 条件 | 环境变量/fixture | 影响用例 |
|------|-----------------|---------|
| 无硬件 | `HARDWARE_AVAILABLE=false` | 全部 HW 用例 skip |
| 非 335Le | `DEVICE_TYPE!=gemini-335le` | 02_03, 02_04, 03_05, 04_03 |
| 单设备 | `DEVICE_COUNT<2` | 19_03 |
| 非破坏性 | `ALLOW_DESTRUCTIVE_TESTS!=true` | 02_04, 02_05, 14_07, 21_07~09 |
| 无回放文件 | `PLAYBACK_BAG_PATH` 未设 | 18_03~07 |
