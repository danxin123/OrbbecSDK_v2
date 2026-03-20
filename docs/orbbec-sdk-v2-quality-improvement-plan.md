# Orbbec SDK V2 质量提升方案

> 对标 ZED / RealSense / Azure Kinect 世界一流 SDK 质量管控体系，结合 Orbbec SDK V2 现状制定的体系化质量提升方案。
>
> 版本范围：OrbbecSDK_v2 (C++)、pyorbbecsdk (Python)、OrbbecSDK_ROS1、OrbbecSDK_ROS2
>
> 更新日期：2026-03-20

---

## 一、背景与约束

| 项目 | 说明 |
|------|------|
| **团队** | 15 人总计，仅 1 人专职质量 |
| **核心痛点** | ROS Wrapper 质量跟不上 Core SDK；文档/示例与实际 API 不匹配；缺少自动化门禁，纯人工测试 |
| **CI 基础设施** | 尚未搭建（CI 脚本和 workflow 已设计但未部署） |
| **发版节奏** | 季度发布 |
| **现有资产** | 460+ 测试用例设计文档、CI 脚本已编写、GitHub Actions workflow 已设计 |
| **实际实现差距** | C++ 15 个测试文件（vs 183 个设计）、Python 25 个测试文件（vs 188 个设计） |
| **当前优先** | Gemini 335 USB 主链路 |

---

## 二、竞品对标：差距在哪里

### 2.1 Intel RealSense (librealsense)

| 维度 | RealSense 做法 | Orbbec 现状 | 差距 |
|------|---------------|------------|------|
| PR 门禁 | 每次 PR 自动构建 + 无硬件单元测试 + playback 回归 | 无自动化门禁 | **缺失** |
| Nightly | 多设备真机回归，跨平台 (Win/Linux/ARM) | 无自动执行能力 | **缺失** |
| 文档 | 每个 API 有示例，文档与代码同步更新，Doxygen 自动生成 | 文档命令跑不通 | **严重** |
| 质量文化 | 开发者写测试，QA 负责框架和门禁，每个 PR 必须有对应测试 | 无强制要求 | **缺失** |
| Release | 明确的质量门禁，RC 测试套件，Known Issues 管理 | 纯人工判断 | **缺失** |

### 2.2 Stereolabs ZED SDK

| 维度 | ZED 做法 | Orbbec 现状 | 差距 |
|------|---------|------------|------|
| 文档质量 | 业界标杆，教程 + 示例 + API 文档三位一体 | 文档/示例与 API 脱节 | **严重** |
| 示例一致性 | C++/Python/ROS 示例全部可运行，版本同步更新 | 部分示例跑不通 | **严重** |
| ROS 包装 | 与 Core SDK 版本锁定，launch 文件有完整参数文档 | Wrapper 质量滞后 | **严重** |
| 性能基线 | 公布各机型的性能参数，有基准测试工具 | 基线 CSV 已有但未自动化 | **部分** |

### 2.3 Azure Kinect SDK

| 维度 | Azure Kinect 做法 | Orbbec 现状 | 差距 |
|------|------------------|------------|------|
| 工程规范 | Microsoft 级 CI/CD，严格的代码审查 | 无 CI/CD | **缺失** |
| 测试分层 | 单元/集成/E2E/性能/稳定性完整金字塔 | 测试资产存在但未自动化 | **部分** |
| 发布管控 | 严格的 RC 验证流程，多轮测试 | 无标准化流程 | **缺失** |
| 文档体系 | docs.microsoft.com 级别的文档质量 | 文档命令不可信 | **严重** |

### 2.4 差距总结

| 维度 | RealSense / ZED / Azure Kinect | Orbbec SDK V2 现状 | 差距等级 |
|------|-------------------------------|-------------------|---------|
| PR 门禁 | 每次 PR 自动构建+测试+回归，<30min | 无自动化门禁 | **缺失** |
| 开发者测试 | PR 必须附带测试代码 | 无强制要求 | **缺失** |
| 文档-示例一致性 | API/示例/文档三位一体，CI 自动校验 | 文档命令跑不通 | **严重** |
| ROS Wrapper | 与 Core SDK 版本锁定，launch 有完整参数文档 | Wrapper 质量滞后 | **严重** |
| Playback 回归 | 录制数据驱动，无需硬件即可回归 | 框架已有但未纳入 CI | **部分** |
| 真机 Nightly | 多设备+多平台每日自动回归 | 无自动执行能力 | **缺失** |
| 性能基线 | 公布基准参数，3σ 统计回归检测 | 基线 CSV 已有但未自动化 | **部分** |
| 发版门禁 | RC 套件通过才能发版，审批流自动化 | 纯人工判断 | **缺失** |

---

## 三、方案设计原则

1. **自动化优先**：1 人管质量不可能手工覆盖 4 个仓库 × 多平台，必须让机器干活
2. **开发者写测试**：14 个开发者是最大的测试产能，质量人员建框架和门禁，开发者为自己的代码写测试
3. **渐进式推进**：先跑通最小闭环（PR 门禁），再逐步扩展到 Daily Smoke → Nightly → Release Gate

---

## 四、优先级排序 (P0 → P3)

### P0：自动化门禁基础 — 阻塞发版的根因

> 没有自动化门禁意味着每次发版都是在赌运气

| 序号 | 提升项 | 具体内容 | 说明 |
|------|--------|---------|------|
| 1 | 搭建 GitHub Actions PR 门禁 | 创建 `pr-gate.yml`，Linux + Windows 构建，C++ 无硬件测试（15 个 GoogleTest），Python 无硬件测试（25 个 pytest），<30 分钟 | 质量人员搭建，开发者配合 |
| 2 | 建立"开发者写测试"规范 | 新 PR 必须包含对应测试代码（或声明豁免），CONTRIBUTING.md 明确要求，Code Review Checklist 增加"测试覆盖" | 改变文化 |
| 3 | Playback 回归测试 | 录制 Gemini 335 参考 .bag 数据，利用现有 `tests/playback/` 框架，纳入 PR 门禁，零硬件依赖 | ROI 最高 |

### P1：文档-示例-API 一致性治理 — 用户最直接感受

> 用户反馈文档/示例跑不通是最大的体验问题

| 序号 | 提升项 | 具体内容 | 说明 |
|------|--------|---------|------|
| 4 | 文档自动检查 | 创建 `docs-check.yml`，Markdown 链接检查（mlc-config.json 已配置），API 文档与实际接口映射检查，纳入 PR 门禁 | 死链直接 fail |
| 5 | 示例可编译/可运行检查 | 创建 `examples-smoke.yml`，C++ 示例全部可编译，Python 示例语法+import 检查，ROS launch 合法性检查 | PCL/Open3D 标记 optional |
| 6 | 建立 API-示例-文档四向映射表 | 基于已有功能矩阵（380+ 功能点），标记"有 API 无示例"、"有示例无文档"等缺口，作为发版前必查资产 | 持续维护 |

### P2：ROS Wrapper 质量对齐 — 核心痛点

> ROS Wrapper 是机器人用户的主入口，但需要先有 CI 基础

| 序号 | 提升项 | 具体内容 | 说明 |
|------|--------|---------|------|
| 7 | ROS1/ROS2 构建门禁 | Docker 化 ROS 环境，colcon/catkin_make 构建，launch 参数解析，纳入 PR 门禁 | 不需要真机 |
| 8 | ROS topic/param 基线检查 | 建立默认 launch 的 topic/param 列表基线，每次 PR 自动对比，发现意外变更 | 防止回归 |
| 9 | ROS Wrapper 版本锁定机制 | Core SDK 版本与 ROS Wrapper 版本对齐，子模块更新时触发 ROS 构建验证 | 防止脱节 |

### P3：真机回归与性能基线 — 深度质量保障

> 需要 self-hosted runner 基础设施，投入大但长期价值最高

| 序号 | 提升项 | 具体内容 | 说明 |
|------|--------|---------|------|
| 10 | 搭建 self-hosted runner + 真机环境 | 至少 1 台 Linux + Gemini 335 USB，设备健康检查脚本，runner 自动清理重启 | 硬件投入 |
| 11 | Daily Smoke 自动化 | 创建 `daily-hw-smoke.yml`，单设备枚举/开流/取帧/点云/录制回放，每日 UTC 02:00，通过率 ≥95% | 问题当天发现 |
| 12 | 性能基准自动回归 | 创建 `nightly-regression.yml`，FPS/CPU/内存/丢帧率基线比较（3σ 回归检测），2 小时稳定性测试 | 统计方法 |
| 13 | 发版质量门禁 | 创建 `release-candidate.yml`，P0 用例 100% 通过，总通过率 ≥95%，benchmark 无显著回退，自动生成发布质量报告 | 审批流 |

---

## 五、质量看板设计

### 5.1 技术选型

| 组件 | 方案 | 理由 |
|------|------|------|
| 前端 | GitHub Pages (gh-pages 分支) | 零成本、零运维 |
| 图表 | Chart.js (CDN) | 一个 JS 文件搞定趋势图 |
| 数据 | data.json 文件 | CI 每次跑完自动 append 一条记录 |
| 更新 | update-dashboard.yml | 复用现有 `test_report_aggregator.py` 的趋势分析能力 |

### 5.2 看板五大板块

**板块 1 — 构建状态总览**（首页顶部卡片）
- 4 个仓库的构建 Badge（C++ / pyorbbecsdk / ROS1 / ROS2）
- 最近一次 PR Gate / Daily Smoke / Nightly 通过率
- 距下次季度发版倒计天数

**板块 2 — 通过率趋势图**（核心，Chart.js 折线图）
- Daily Smoke 30 天通过率趋势（门禁线 95% 红色虚线）
- Nightly 30 天通过率趋势（门禁线 90%）
- 区分基础设施失败 vs 测试失败

**板块 3 — 模块健康度矩阵**

| 模块 | 用例设计 | 已实现 | 实现率 | 最新通过率 | 健康度 |
|------|---------|--------|--------|-----------|--------|
| 01 Context | 6 | 2 | 33% | 100% | 🟡 |
| 08 Pipeline | 10 | 3 | 30% | 90% | 🟡 |
| 13 Filter | 15 | 1 | 7% | — | 🔴 |
| ROS2 | 9 | 1 | 11% | — | 🔴 |

健康度规则：🟢 实现率>60% 且通过率>95% | 🟡 其一不达 | 🔴 实现率<20% 或通过率<80%

**板块 4 — 失败用例跟踪**
- 连续失败 >3 天的用例（高亮，需立即关注）
- 最近 24 小时新增失败
- 基础设施失败 vs 测试失败区分

**板块 5 — 发版就绪度仪表盘**
- P0 用例通过率（门禁：100%）
- 整体通过率（门禁：≥98%）
- 文档链接检查状态
- 示例可编译率
- 未关闭 P0 Bug 数量

### 5.3 需新建文件

| 文件 | 作用 |
|------|------|
| `ci/scripts/update_dashboard.py` | 读取最新报告 → 更新 data.json |
| `.github/workflows/update-dashboard.yml` | CI 完成后触发看板更新 |
| `docs/dashboard/index.html` + `data.json` | 看板前端 |

---

## 六、测试用例实现节奏（335 USB 主链路优先）

### 6.1 总体策略

- 460+ 用例已设计，当前实现不到 10%
- 目标补齐 ~150 个核心用例，覆盖 335 USB 全部 P0 功能
- 质量人员负责框架和 Review，开发者每人每周期认领 3-5 个用例
- 进度在质量看板"模块健康度矩阵"实时可见

### 6.2 批次规划

#### 批次 1：基础模块 — ~40 个用例

| 模块 | 用例范围 | 数量 | 说明 |
|------|---------|------|------|
| 01 Context | TC_CPP_01_01~06, TC_PY_100~104 | 11 | 默认/配置文件构造、重复创建销毁、内存释放 |
| 08 Pipeline | TC_CPP_08_01~08, TC_PY_130~138 | 17 | 创建/启动/停止/取帧/超时/重复开关流 |
| 02 Discovery | TC_CPP_02_01~06, TC_PY_105~112 | 12 | USB 枚举、热插拔、时钟同步 |

#### 批次 2：数据链路 — ~45 个用例

| 模块 | 数量 | 说明 |
|------|------|------|
| 10 Frame | 15 | 帧数据完整性、格式校验、时间戳单调递增 |
| 06 Sensor | 10 | Depth/Color/IR/IMU 传感器枚举完整性 |
| 07 StreamProfile | 10 | 分辨率/帧率/格式配置有效性 |
| 13 Filter | 10 | 点云生成、D2C 对齐、格式转换 |

#### 批次 3：高级功能 — ~35 个用例

| 模块 | 数量 | 说明 |
|------|------|------|
| 15 DepthWorkMode | 8 | 深度工作模式枚举/切换/生效验证 |
| 16 Preset | 8 | 预设导入/导出/列表/应用 |
| 18 Record/Playback | 10 | 录制→回放→帧数据一致性校验 |
| 20 CoordinateTransform | 9 | D2C、3D→2D 投影、坐标系一致性 |

#### 批次 4：补齐 + ROS — ~30 个用例

| 模块 | 数量 | 说明 |
|------|------|------|
| 14 Property | 10 | 设备属性读写、只读/可写/范围校验 |
| 24 ErrorHandling | 8 | 非法参数、空指针、超时、重复操作 |
| ROS1 + ROS2 | 12 | launch 冒烟、topic 基线、param 一致性 |

### 6.3 实现管理规则

- C++ 和 Python 双语言同步推进
- 新增用例必须注册到 `ci/configs/test-registry.yaml`
- 使用 `tests/conftest.py` 中的共享 fixture
- 进度在质量看板实时可见

---

## 七、关键文件映射

### 已有可复用资产

| 文件 | 作用 |
|------|------|
| `tests/conftest.py` | 共享 fixture、设备生命周期管理 |
| `tests/cpp/test_01~15*.cpp` | 15 个 C++ GoogleTest 文件 |
| `tests/python/test_01~25*.py` | 25 个 Python pytest 文件 |
| `tests/hardware/test_gemini335_p0.py` | P0 真机测试 |
| `tests/playback/test_playback_regression.py` | 回放回归 |
| `ci/scripts/*.sh` | 构建/测试/检查脚本 |
| `ci/configs/test-registry.yaml` | 测试用例注册表 |
| `ci/configs/test_suite.yaml` | 门禁阈值配置 |
| `ci/configs/benchmark_baseline.csv` | 性能基线 |
| `docs/testcases/` | 460+ 用例设计文档 |
| `docs/sdk_feature_matrix.md` | 380+ 功能点矩阵 |

### 需要新建

| 文件 | 作用 |
|------|------|
| `.github/workflows/pr-gate.yml` | PR 门禁 workflow |
| `.github/workflows/docs-check.yml` | 文档检查 |
| `.github/workflows/examples-smoke.yml` | 示例检查 |
| `.github/workflows/daily-hw-smoke.yml` | 每日冒烟 |
| `.github/workflows/nightly-regression.yml` | 夜间回归 |
| `.github/workflows/release-candidate.yml` | 发版门禁 |
| `.github/workflows/update-dashboard.yml` | 看板更新 |
| `CONTRIBUTING.md` | 开发者测试规范 |
| `docs/dashboard/index.html` + `data.json` | 质量看板 |
| `ci/scripts/update_dashboard.py` | 看板数据更新 |
| ROS1/ROS2 Dockerfile | ROS 构建环境 |
| `.github/RELEASE_CHECKLIST.md` | 发版检查表 |

---

## 八、验证标准

| 序号 | 验证项 | 达标标准 |
|------|--------|---------|
| 1 | PR 门禁 | 提交 PR 后 30 分钟内自动返回构建+测试结果 |
| 2 | 文档检查 | 死链数量降为 0 |
| 3 | 示例检查 | C++ 示例 100% 可编译，Python 示例 100% 可 import |
| 4 | ROS 构建 | ROS1/ROS2 workspace 构建成功率 100% |
| 5 | Daily Smoke | 通过率 ≥95%，每日自动执行 |
| 6 | 发版门禁 | P0 用例全部通过后才可进入 RC |
| 7 | 质量看板 | gh-pages 站点可访问，趋势图有数据 |
| 8 | 用例实现 | 核心 150 个用例在 CI 中自动执行 |
| 9 | 模块健康 | 所有 P0 模块健康度从 🔴 提升到 🟢 |

---

## 九、核心决策

| 决策 | 理由 |
|------|------|
| 自动化是唯一出路 | 1 人专职质量不可能手工覆盖 4 个仓库 × 多平台 |
| 开发者必须写测试 | 14 个开发者是最大的测试产能，QA 建框架不写用例 |
| 先 hosted runner 后 self-hosted | 无硬件 CI 可立刻开始，真机环境后续搭建 |
| 335 USB 主链路优先 | 先聚焦，335Le/335Lg 专项留待第二季度 |
| 看板用 GitHub Pages | 零成本零运维，不引入 Grafana 等重型工具 |
| Playback 是 ROI 最高的投入 | 一次录制，无限回归，零硬件成本 |
