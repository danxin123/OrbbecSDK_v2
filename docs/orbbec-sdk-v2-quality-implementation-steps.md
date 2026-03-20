# Orbbec SDK V2 质量提升 — 具体实施步骤

> 本文档是《Orbbec SDK V2 质量提升方案》的落地执行手册。
> 按依赖关系排序，每步完成后才进入下一步。不含时间周期，只讲"干什么、怎么干、产出什么、依赖什么"。
>
> 更新日期：2026-03-20

---

## 步骤依赖总览

```
步骤 1 (PR Gate) ─┬─→ 步骤 2 (分支保护)
                  ├─→ 步骤 3 (Playback 数据)
                  ├─→ 步骤 4 (开发者规范)
                  ├─→ 步骤 6 (示例检查, 需构建产物)
                  ├─→ 步骤 17~20 (用例实现, 需 CI 执行)
                  └─→ 步骤 12~13 (看板数据, 需 CI 报告)

步骤 5 (文档检查) ──→ 无前置依赖，可与步骤 1 并行

步骤 7 (映射表) ──→ 无前置依赖，可与步骤 1 并行

步骤 8 (ROS Docker) → 步骤 9 (ROS PR 门禁) → 步骤 10 (topic 基线)

步骤 11 (看板骨架) → 步骤 12 (更新脚本) → 步骤 13 (自动更新 workflow)

步骤 14 (runner) → 步骤 15 (Daily Smoke) → 步骤 16 (Nightly)
                                            → 步骤 21 (RC workflow)
                                              → 步骤 22 (checklist)
```

---

## 阶段一：PR 自动化门禁

> 最先做，后续所有步骤依赖它

### 步骤 1：在 OrbbecSDK_v2 仓库部署 PR Gate workflow

**做什么**：创建 `.github/workflows/pr-gate.yml`，配置 5 个 Job

**怎么干**：

- **Job 1 `build-linux`**
  - Runner：ubuntu-22.04 hosted runner
  - 调用现有 `ci/scripts/build_sdk.sh`
  - 环境变量：`BUILD_TYPE=Release OB_BUILD_TESTS=ON`

- **Job 2 `build-windows`**
  - Runner：windows-2022 hosted runner
  - 调用现有 `ci/scripts/build_sdk.ps1`

- **Job 3 `test-cpp-nohw`**
  - 依赖 Job 1 产物
  - 执行 `ctest` 跑 `tests/cpp/` 下 15 个 GoogleTest 文件
  - 环境变量：`HARDWARE_AVAILABLE=false`

- **Job 4 `test-python-nohw`**
  - 执行 `pytest tests/python -m "not hardware" --timeout=60`
  - 跑 `tests/python/` 下 25 个文件

- **Job 5 `test-playback`**
  - 执行 `pytest tests/playback/ -m playback --timeout=60`
  - 跑 `tests/playback/test_playback_regression.py`

**产出**：每次 PR 自动跑构建+测试，结果写入 GitHub Step Summary

**验收**：提交一个测试 PR，30 分钟内看到自动构建结果

**依赖**：无

---

### 步骤 2：开启分支保护规则

**做什么**：在 GitHub 仓库 Settings → Branches → main 分支设置 Branch Protection

**怎么干**：

1. 勾选 "Require status checks to pass before merging"
2. 添加 `build-linux`、`test-cpp-nohw`、`test-python-nohw` 为 required checks
3. 勾选 "Require pull request reviews before merging"（至少 1 人 review）
4. 禁止直接 push main

**产出**：构建或测试失败的 PR 无法合入 main

**验收**：故意提交一个会导致测试失败的 PR，确认无法合并

**依赖**：步骤 1

---

### 步骤 3：录制 Gemini 335 参考数据，充实 Playback 回归

**做什么**：用真机录制 .bag 参考文件，存入 `tests/playback/data/`

**怎么干**：

1. 连接 Gemini 335 USB 设备
2. 运行录制脚本：开流 Depth+Color+IR 10 秒，生成 `gemini335_reference.bag`
3. 在 `tests/playback/test_playback_regression.py` 中添加对该参考文件的帧完整性、时间戳单调性、数据格式校验
4. 将 .bag 文件提交到仓库（或用 Git LFS）

**产出**：PR Gate 中的 playback 测试从空跑变为真正有回归价值

**验收**：在无设备的 hosted runner 上成功回放并校验帧数据

**依赖**：步骤 1

---

### 步骤 4：建立开发者写测试的规范和流程

**做什么**：编写 CONTRIBUTING.md 中的测试要求章节

**怎么干**：

- **规则 1**：新增/修改 API 的 PR 必须附带对应测试代码（C++ 或 Python），否则需在 PR 描述中声明豁免理由
- **规则 2**：测试函数命名遵循 `test_TC_<模块>_<序号>_<描述>` 格式（与 `docs/testcases/` 中的 TC_ID 对齐）
- **规则 3**：使用 `tests/conftest.py` 中的共享 fixture（`ob_context`、`ob_pipeline`、`require_hardware` 等）
- **规则 4**：新用例必须注册到 `ci/configs/test-registry.yaml`
- 提供 2 个模板示例：一个无硬件测试 + 一个硬件测试
- 在 Code Review Checklist 增加"是否包含测试"检查项

**产出**：CONTRIBUTING.md 更新，开发者有明确的测试编写指引

**验收**：随后的 PR 中开始看到开发者提交的测试代码

**依赖**：步骤 1（有门禁才有执行环境）

---

## 阶段二：文档与示例自动校验

> 可与阶段一部分并行

### 步骤 5：部署文档链接检查 workflow

**做什么**：创建 `.github/workflows/docs-check.yml`

**怎么干**：

1. 调用现有 `ci/scripts/run_docs_checks.sh`
2. 使用已配置的 `ci/configs/mlc-config.json` 做 Markdown 链接检查
3. 触发条件：PR 中修改了 `docs/`、`README.md`、`*.md` 文件时
4. 死链直接报 failure

**产出**：文档中的死链、失效图片、错误锚点在 PR 阶段就被拦截

**验收**：故意加一个死链，确认 workflow 报错

**依赖**：无（可与步骤 1 并行）

---

### 步骤 6：部署示例可编译检查 workflow

**做什么**：创建 `.github/workflows/examples-smoke.yml`

**怎么干**：

1. 调用现有 `ci/scripts/run_examples.sh`
2. **Job 1**：Linux 上编译所有 C++ examples（`OB_BUILD_EXAMPLES=ON`）
3. **Job 2**：Windows 上编译所有 C++ examples
4. **Job 3**：对所有 Python examples 执行 `python -m py_compile <file>` + `import` 检查
5. PCL/Open3D 依赖的示例标记为 optional，失败不阻塞 PR

**产出**：示例编译失败在合入前就被发现

**验收**：修改一个 example 引入编译错误，确认 workflow 报红

**依赖**：步骤 1 的构建产物（C++ examples 需要 SDK 库）

---

### 步骤 7：建立 API-示例-文档四向映射表

**做什么**：基于现有 `docs/sdk_feature_matrix.md` 扩展，增加"示例"和"文档"列

**怎么干**：

1. 遍历 `docs/sdk_feature_inventory.md` 中的 380+ 功能点
2. 对每个功能点标记：有 API（✅/❌）、有示例（✅/❌）、有文档（✅/❌）、有回归测试（✅/❌）
3. 生成缺口清单：列出"有 API 无示例"、"有示例无文档"、"有文档但命令过期"的条目
4. 按 P0/P1/P2 优先级排列缺口

**产出**：一份映射表 + 缺口清单，作为后续补齐的参照

**验收**：映射表中包含全部 380+ 功能点的四维状态

**依赖**：无（纯文档工作）

---

## 阶段三：ROS Wrapper 质量对齐

> 依赖阶段一完成

### 步骤 8：Docker 化 ROS 构建环境

**做什么**：为 ROS1 Noetic 和 ROS2 Humble 各创建 CI 构建用的 Dockerfile

**怎么干**：

1. **ROS1 Dockerfile**：基于 `ros:noetic-ros-base`，预装 catkin 工具链 + pyorbbecsdk 依赖
2. **ROS2 Dockerfile**：基于 `ros:humble-ros-base`，预装 colcon 工具链
3. 在 CI 中用 `docker build` + `docker run` 执行 ROS 构建，不依赖 runner 本机 ROS 环境
4. 镜像推送到 GitHub Container Registry (ghcr.io) 缓存

**产出**：两个 Docker 镜像，可在任意 hosted runner 上执行 ROS 构建

**验收**：在 ubuntu-22.04 hosted runner 上成功执行 `catkin_make` 和 `colcon build`

**依赖**：步骤 1（CI 基础设施就绪）

---

### 步骤 9：ROS 构建 + launch 检查纳入 PR 门禁

**做什么**：在 pr-gate.yml 中新增 ROS 相关 Job

**怎么干**：

1. **Job `ros1-build`**：在 ROS1 Docker 中执行 `catkin_make`，调用 `ci/scripts/run_ros1_tests.sh`
2. **Job `ros2-build`**：在 ROS2 Docker 中执行 `colcon build && colcon test`，调用 `ci/scripts/run_ros2_tests.sh`
3. 两个 Job 验证：构建成功 + launch 文件格式合法 + 参数声明无报错
4. 将 `ros1-build` 和 `ros2-build` 加入 required status checks

**产出**：ROS Wrapper 的改动在合入前必须通过构建验证

**验收**：修改一个 ROS launch 文件引入错误，确认 PR 被阻止合并

**依赖**：步骤 8

---

### 步骤 10：建立 ROS topic/param 基线并自动对比

**做什么**：创建 ROS 默认 launch 的 topic 和 param 基线文件

**怎么干**：

1. 用真机启动默认 launch，抓取 `rostopic list` / `ros2 topic list` + `rosparam list` / `ros2 param list` 输出
2. 存为 `tests/ros1/baseline_topics.txt`、`tests/ros1/baseline_params.txt`（ROS2 同理）
3. 在 `tests/ros1/` 和 `tests/ros2/` 中编写对比测试：CI 中启动 launch → 等待节点 ready → 对比 topic/param 列表与基线 → 差异报错
4. 纳入 nightly 或 daily smoke

**产出**：任何意外的 topic 名称变更、参数默认值变更都会被自动发现

**验收**：故意修改一个 topic 名称，确认 CI 报出 diff

**依赖**：步骤 8、步骤 9

---

## 阶段四：质量看板搭建

> 可与阶段二并行推进

### 步骤 11：创建 GitHub Pages 看板骨架

**做什么**：在仓库中创建 `docs/dashboard/index.html` + 开启 GitHub Pages

**怎么干**：

1. 创建 gh-pages 分支（或使用 main 分支的 `docs/dashboard/` 目录）
2. `index.html`：一个静态 HTML 页面，引入 Chart.js CDN
3. 包含 5 个板块骨架：构建 Badge 区、趋势图区、模块矩阵区、失败跟踪区、发版就绪度区
4. `data.json`：初始为空数组 `[]`，后续 CI 自动往里追加数据
5. 在 GitHub Settings → Pages 中启用

**产出**：一个可访问的空白看板页面 URL

**验收**：打开 `https://<org>.github.io/<repo>/dashboard/` 能看到页面

**依赖**：无

---

### 步骤 12：编写看板数据更新脚本

**做什么**：创建 `ci/scripts/update_dashboard.py`

**怎么干**：

1. **输入**：CI 产出的测试报告 JSON（来自 `ci/scripts/test_report_aggregator.py` 或 pytest/CTest XML）
2. **处理**：解析通过率、失败用例列表、模块维度统计，追加一条记录到 `data.json`
3. 保留最近 90 天数据（旧数据自动清理）
4. **模块健康度计算**：从 `ci/configs/test-registry.yaml` 读取用例设计总数，与实际执行数对比
5. 健康度规则：🟢 实现率>60% 且通过率>95% | 🟡 其一不达 | 🔴 实现率<20% 或通过率<80%

**产出**：`update_dashboard.py` 脚本

**验收**：手动传入一份测试报告 JSON，确认 `data.json` 正确更新

**依赖**：步骤 11（页面骨架）、步骤 1（有 CI 报告产出）

---

### 步骤 13：创建看板自动更新 workflow

**做什么**：创建 `.github/workflows/update-dashboard.yml`

**怎么干**：

1. 触发条件：`workflow_run` — 在 pr-gate / daily-hw-smoke / nightly-regression 完成后触发
2. 步骤：下载上游 workflow 的报告 artifact → 运行 `update_dashboard.py` → 将更新后的 `data.json` push 到 gh-pages 分支
3. 使用 `GITHUB_TOKEN` 推送（需给 workflow 写权限）

**产出**：每次 CI 跑完后看板自动更新

**验收**：跑一次 PR Gate，看板趋势图出现新数据点

**依赖**：步骤 12

---

## 阶段五：真机 Daily Smoke

> 依赖 runner 硬件准备

### 步骤 14：搭建第一台 self-hosted runner

**做什么**：准备一台 Ubuntu 22.04 机器 + Gemini 335 USB 接入

**怎么干**：

1. 安装 GitHub Actions runner 并注册到仓库
2. 标签：`self-hosted, linux, sdk, usb, gemini-335`
3. 安装 SDK 依赖：CMake、GCC、Python 3.9+、udev 规则
4. 部署 `ci/scripts/device_health_check.sh` 作为 cron 任务
5. 验证 `lsusb | grep 2bc5` 能发现设备
6. 参考 `ci/configs/runner-labels.json` 配置标签

**产出**：一台就绪的 self-hosted runner，设备在线

**验收**：在 GitHub Actions UI 看到 runner 在线且标签正确

**依赖**：硬件采购/分配

---

### 步骤 15：部署 Daily Smoke workflow

**做什么**：创建 `.github/workflows/daily-hw-smoke.yml`

**怎么干**：

1. **触发**：`schedule: cron '0 2 * * *'`（UTC 02:00）+ `workflow_dispatch`（手动触发）
2. **前置检查**：运行 `device_health_check.sh`，设备不在线则标记 `INFRA_FAIL` 并跳过
3. **Job 1 `cpp-hw-smoke`**：构建 SDK → 运行 `ctest` 含硬件标记的用例
4. **Job 2 `python-hw-smoke`**：`pytest tests/python -m hardware --device-type=gemini-335`
5. **Job 3 `p0-gate`**：执行 `tests/hardware/test_gemini335_p0.py` — 枚举/开流/取帧/点云/录制回放
6. **结果汇总**：调用 `reusable-test-report.yml` 生成 Step Summary
7. **通过率门禁**：低于 95% 发送告警（GitHub Actions 通知或 webhook）

**产出**：每天自动验证 335 USB 主链路，问题当天发现

**验收**：连续 3 天自动执行且报告正常生成

**依赖**：步骤 14

---

### 步骤 16：部署 Nightly Regression workflow

**做什么**：创建 `.github/workflows/nightly-regression.yml`

**怎么干**：

1. **触发**：`schedule: cron '0 18 * * *'`（UTC 18:00）
2. **Job 1 `benchmark`**：调用 `ci/scripts/run_benchmark.sh` 运行多轮性能采集 → 调用 `ci/scripts/analyze_benchmark.py` 与 `ci/configs/benchmark_baseline.csv` 做 3σ 回归检测
3. **Job 2 `stability-2h`**：运行 `tests/benchmark/test_stability.py --duration 7200`
4. **Job 3 `scenarios`**：运行 `tests/scenarios/` 下的 3D 测量、避障、SLAM 测试
5. **汇总**：区分 `INFRA_FAIL`（设备问题）vs `TEST_FAIL`（SDK bug），有效通过率门禁 90%
6. 将 benchmark.csv 上传 artifact（保留 30 天）

**产出**：每天深度回归 + 性能趋势数据

**验收**：benchmark 结果出现在质量看板趋势图中

**依赖**：步骤 14、步骤 15

---

## 阶段六：测试用例批量补齐

> 持续进行，与上述阶段穿插

### 步骤 17：实现基础模块用例（~40 个）

**做什么**：按 `docs/testcases/` 中的设计文档，实现 Context、Pipeline、Discovery 三个模块的 C++ 和 Python 测试

**具体范围**：

- **模块 01 Context**：实现 TC_CPP_01_01~06 + TC_PY_100~104（共 11 个）
  - 默认构造/析构、配置文件构造、重复创建销毁、空闲内存释放
  - C++ 放 `tests/cpp/test_01_context.cpp`，Python 放 `tests/python/test_01_context.py`

- **模块 08 Pipeline**：实现 TC_CPP_08_01~08 + TC_PY_130~138（共 17 个）
  - 创建/启动/停止/取帧/超时/配置变更/重复开关流

- **模块 02 Discovery**：实现 TC_CPP_02_01~06 + TC_PY_105~112（共 12 个）
  - USB 枚举、热插拔回调、时钟同步

- 每个用例注册到 `ci/configs/test-registry.yaml`

**产出**：40 个新增可执行测试用例

**验收**：PR Gate 中这些用例全部通过

**依赖**：步骤 1（有 CI 执行环境）、步骤 4（有编写规范）

---

### 步骤 18：实现数据链路用例（~45 个）

**做什么**：Frame、Sensor、StreamProfile、Filter 四个模块

**具体范围**：

- **10 Frame**（15 个）：帧数据有效性、格式校验、时间戳单调递增、metadata 读取
- **06 Sensor**（10 个）：传感器列表完整性、按类型获取、Depth/Color/IR/IMU 全覆盖
- **07 StreamProfile**（10 个）：支持的分辨率/帧率/格式列表查询、VideoStreamProfile 属性校验
- **13 Filter**（10 个）：PointCloudFilter、Align、FormatConvert、HdrMerge、空间/时间滤波

**产出**：45 个新增用例，覆盖"开流后数据是否正确"这条完整链路

**验收**：Daily Smoke 中所有数据链路用例通过

**依赖**：步骤 17（基础模块就绪）

---

### 步骤 19：实现高级功能用例（~35 个）

**做什么**：DepthWorkMode、Preset、Record/Playback、CoordinateTransform

**具体范围**：

- **15 DepthWorkMode**（8 个）：模式枚举、切换、切换后帧数据验证
- **16 Preset**（8 个）：preset 列表查询、导入导出、应用后生效验证
- **18 Record/Playback**（10 个）：录制生成文件、回放恢复帧数据、录制文件完整性
- **20 CoordinateTransform**（9 个）：D2C 对齐、3D→2D 投影、坐标系一致性

**产出**：35 个新增用例

**验收**：Daily Smoke 和 Nightly 中用例通过

**依赖**：步骤 18

---

### 步骤 20：实现补齐模块 + ROS 用例（~30 个）

**做什么**：Property、ErrorHandling、ROS1/ROS2

**具体范围**：

- **14 Property**（10 个）：只读属性获取、可写属性设置+读回验证、范围越界处理
- **24 ErrorHandling**（8 个）：空指针传入、非法参数、超时场景、重复 stop、销毁后调用
- **ROS1**（6 个）：launch 启动+节点存活、topic 基线对比、param 一致性
- **ROS2**（6 个）：launch 启动、topic/service 验证、QoS 兼容性

**产出**：30 个新增用例，累计达到 ~150 个核心用例

**验收**：所有 P0 模块健康度从 🔴 提升到 🟢

**依赖**：步骤 9（ROS CI 就绪）、步骤 10（基线建立）

---

## 阶段七：发版质量门禁

> 所有阶段完成后的收官

### 步骤 21：部署 Release Candidate workflow

**做什么**：创建 `.github/workflows/release-candidate.yml`

**怎么干**：

1. **触发**：`workflow_dispatch`（手动），限 release/* 分支
2. 执行全部测试套件：commit_gate + daily_smoke + nightly 的全集
3. **门禁阈值**（来自 `ci/configs/test_suite.yaml`）：
   - P0 用例 100% 通过
   - 总通过率 ≥98%
   - benchmark 无 3σ 回退
4. 调用 `ci/scripts/test_report_aggregator.py` 生成发布质量报告
5. 绑定 GitHub Environment `rc-validation`，配置 required reviewers
6. 自动生成 Release Notes 草稿（通过率、失败摘要、Known Issues）

**产出**：一键执行 RC 验证 + 自动生成质量报告 + 审批流

**验收**：在 release 分支上触发，审批通过后才能进入正式发布

**依赖**：步骤 15、步骤 16（Daily Smoke + Nightly 稳定运行）

---

### 步骤 22：建立发版 checklist 和 Known Issues 模板

**做什么**：在仓库中创建 `.github/RELEASE_CHECKLIST.md` 和 `KNOWN_ISSUES.md` 模板

**怎么干**：

Checklist 包含：
- [ ] 4 个仓库构建绿灯（C++ / Python / ROS1 / ROS2）
- [ ] P0 用例全通过
- [ ] 文档链接检查通过
- [ ] 示例编译通过
- [ ] API-示例-文档映射表无新增缺口
- [ ] benchmark 无回退
- [ ] Known Issues 已更新
- [ ] Release Notes 已审阅

每次发版前由质量人员按 checklist 逐项确认，RC workflow 自动填充可自动化的项。

**产出**：发版流程标准化，不再依赖记忆和口头确认

**验收**：首次使用标准化流程完成一次发版

**依赖**：步骤 21

---

## 关键文件总览

### 需要创建的文件

| 文件 | 对应步骤 | 说明 |
|------|---------|------|
| `.github/workflows/pr-gate.yml` | 步骤 1 | PR 门禁 workflow |
| `.github/workflows/docs-check.yml` | 步骤 5 | 文档链接检查 |
| `.github/workflows/examples-smoke.yml` | 步骤 6 | 示例可编译检查 |
| `.github/workflows/daily-hw-smoke.yml` | 步骤 15 | 每日真机冒烟 |
| `.github/workflows/nightly-regression.yml` | 步骤 16 | 夜间深度回归 |
| `.github/workflows/release-candidate.yml` | 步骤 21 | 发版质量门禁 |
| `.github/workflows/update-dashboard.yml` | 步骤 13 | 看板自动更新 |
| `CONTRIBUTING.md`（测试规范章节） | 步骤 4 | 开发者测试指引 |
| `docs/dashboard/index.html` + `data.json` | 步骤 11 | 质量看板前端 |
| `ci/scripts/update_dashboard.py` | 步骤 12 | 看板数据更新脚本 |
| ROS1/ROS2 Dockerfile | 步骤 8 | ROS 构建环境 |
| `tests/ros1/baseline_topics.txt` 等 | 步骤 10 | ROS 基线文件 |
| `.github/RELEASE_CHECKLIST.md` | 步骤 22 | 发版检查表 |

### 已有可直接复用的文件

| 文件 | 作用 |
|------|------|
| `ci/scripts/build_sdk.sh` / `.ps1` | 构建入口 |
| `ci/scripts/run_python_tests.sh` | pytest runner |
| `ci/scripts/run_cpp_tests.sh` | CTest runner |
| `ci/scripts/run_docs_checks.sh` | 文档检查 |
| `ci/scripts/run_examples.sh` | 示例检查 |
| `ci/scripts/run_ros1_tests.sh` / `run_ros2_tests.sh` | ROS 测试 |
| `ci/scripts/device_health_check.sh` | 设备自检 |
| `ci/scripts/analyze_benchmark.py` | 3σ 回归检测 |
| `ci/scripts/test_report_aggregator.py` | 报告聚合 |
| `tests/conftest.py` | 共享 fixture 框架 |
| `ci/configs/test-registry.yaml` | 用例注册表 |
| `ci/configs/test_suite.yaml` | 门禁阈值 |
| `ci/configs/benchmark_baseline.csv` | 性能基线 |
| `ci/configs/mlc-config.json` | 文档检查配置 |
| `ci/configs/runner-labels.json` | Runner 标签 |
| `ci/configs/artifact-policy.json` | 产物保留策略 |
| `.github/workflows/reusable-test-report.yml` | 报告生成复用 workflow |
| `docs/testcases/` | 460+ 用例设计文档 |
| `docs/sdk_feature_matrix.md` | 380+ 功能点矩阵 |
| `docs/sdk_feature_inventory.md` | SDK 功能清单 |
