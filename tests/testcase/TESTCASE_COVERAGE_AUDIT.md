# Testcase Coverage Audit

更新日期: 2026-03-21

## 范围

基于 `tests/testcase/` 下 TC_CPP_01 ~ TC_CPP_25 设计文档，对当前可执行实现进行分组梳理。

## 现有实现分组

### 1) 无硬件测试 (No-HW)

- 可执行:
  - `tests/core_nohw_test/core_nohw_test.cpp` -> `ob_core_nohw_test`
  - `tests/core_nohw_test/full_feature_matrix_test.cpp` -> `ob_full_feature_matrix_test`
- 当前覆盖的 TC 方向:
  - Context/Discovery: TC_CPP_01, TC_CPP_02
  - Config: TC_CPP_09
  - Version/Log/Error/DataStructures: TC_CPP_22/23/24/25
  - 全量组合回归: TC_CPP_FULL_001 (新增)
- 结论:
  - 具备稳定 no-hardware 主链路验证能力。

### 2) 录制回放测试 (Playback)

- 可执行:
  - `tests/playback_test/playback_smoke_test.cpp` -> `ob_playback_smoke_test`
- 当前覆盖的 TC 方向:
  - Pipeline/Frame/Metadata/FrameFactory: TC_CPP_08/10/11/12 子集
  - RecordPlayback: TC_CPP_18 子集（回放侧）
- 结论:
  - 回放测试深度较好，已覆盖帧属性、元数据、工厂接口和配置切换。

### 3) 硬件测试 (Hardware)

- 可执行:
  - `tests/hardware/test_p0_hardware_smoke.py`
- 当前覆盖的 TC 方向:
  - P0 基础健康检查: USB 可见性 + health script
- 结论:
  - 已具备 RC 硬件 P0 工件来源，但仍为最小 smoke 级别。

## 是否需要补充

需要补充，建议按优先级推进:

1. 硬件 P0 功能链路补充 (高优先级)
- 建议新增:
  - 设备枚举/打开/关闭
  - Pipeline 启停 + 取帧
  - 深度/彩色基础帧属性验证
  - 关键属性读写与越界处理
- 目标:
  - 将硬件 P0 从基础健康检查升级为功能链路门禁。

2. 录制回放补充 (中优先级)
- 建议新增:
  - 多传感器帧同步一致性检查
  - 回放 seek/暂停/恢复稳定性
  - 长时回放内存增长监测

3. 无硬件扩展 (中优先级)
- 建议新增:
  - Filter/Property/CoordinateTransform 更多 API 级别 no-hw 断言
  - 错误类型与错误码矩阵校验

## 本次新增全量功能点用例

- 用例 ID: `TC_CPP_FULL_001_full_feature_matrix`
- 文件: `tests/core_nohw_test/full_feature_matrix_test.cpp`
- 覆盖点:
  - Context/Discovery
  - Config/Version
  - Logging/ErrorHandling
  - DataStructures
  - 可选 playback 分支 (传入 bag 路径时执行)
- 目的:
  - 提供一个跨模块、可快速回归的“全量功能点”综合测试入口。
