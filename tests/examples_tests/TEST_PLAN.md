# Examples 自动化测试方案

## 目标

在 `tests/examples_tests` 下建立一套可持续维护的 examples 自动化测试链路，覆盖 `examples` 目录中除 `application` 外的示例，并满足以下要求：

1. 验证 example 能够被构建并正常启动运行。
2. 对支持 PNG 保存的 example，在自动运行时触发保存，并将产物纳入报告。
3. 按 example README 中声明的支持设备与前置条件，自动分发到匹配的 CI runner。
4. 兼容 Windows、Linux x86_64、Linux ARM64。
5. 生成包含每个测试用例结果的 HTML 报告，同时输出 JUnit XML 以便 CI 展示。

## 总体设计

方案分为 4 层：

1. 元数据层
   - `generate_examples_manifest.py` 扫描 `examples/**/CMakeLists.txt`、`examples/README.md` 和各示例 README。
   - 自动提取 target、依赖、支持设备列表、平台限制、是否支持 PNG 保存、多设备/LiDAR/GMSL/网络等能力标签。
   - 通过 `config/example_overrides.json` 对交互流程、超时、危险操作进行补充或覆盖。

2. 调度层
   - `generate_ci_matrix.py` 基于 manifest 和 `config/runner_pools.json` 生成 CI 矩阵。
   - 每个 runner pool 声明 `platform`、`runs_on`、`device_models`、`device_series`、`capabilities`、`device_count`。
   - 调度器按平台和设备能力做匹配，并将 case 打包成批次，降低单个 job 时长。

3. 执行层
   - `run_examples_suite.py` 负责查找 example 二进制、设置自动化环境变量、注入 stdin、采集日志和图片。
   - 公共 examples 工具层新增测试模式：
     - `OB_EXAMPLE_TEST_MODE=1`
       - `OB_EXAMPLE_AUTO_KEYS=S,ESC` 等自动按键序列
     - `OB_EXAMPLE_TEST_OUTPUT_DIR=<case_artifacts>` 统一保存 PNG 输出
   - 这样 GUI/控制台示例都可以在无人工干预下退出。

4. 报告层
   - 单个 job 生成 `results.json`、`report.html`、`junit.xml`。
   - `merge_example_results.py` 聚合所有 job 的结果，生成总 HTML/JUnit/JSON 报告。

## 关键实现点

### 1. 示例自动退出与自动保存

examples 里大量示例依赖 `waitForKeyPressed()` 或 OpenCV `waitKey()` 退出。为避免为每个 example 单独写特殊分支，公共 `examples/utils` 已增加测试自动按键能力：

- 控制台示例通过 `ob_smpl_wait_for_key_press()` 轮询自动按键。
- OpenCV 示例通过 `CVWindow::run()` 注入自动按键。
- PNG 保存路径通过 `OB_EXAMPLE_TEST_OUTPUT_DIR` 重定向到当前 case 的产物目录。

默认策略：

- 普通 smoke 示例：发送 `ESC`
- 支持 PNG 保存的 viewer：发送 `S,ESC`
- 菜单型示例：通过 override 提供 `stdin_lines` 或定制按键序列

### 2. 设备到 runner 的自动分配

manifest 中每个 case 都会带上：

- `supported_models`
- `supported_series`
- `required_capabilities`
- `min_device_count`
- `supported_platforms`

CI matrix 生成逻辑按以下顺序过滤 runner：

1. 平台匹配
2. 设备数量满足
3. runner capability 覆盖 case capability
4. runner 机型或系列与 case 支持列表相交

如果某个 case 在某个平台上没有可用 runner，会进入 `examples_unassigned.json`，方便运维补 runner 标签或补设备池。

### 3. 风险用例处理

以下用例不应该默认落到普通硬件池：

- `advanced/forceip`：会修改网络设备 IP
- `advanced/optional_depth_presets_update`：会更新设备 preset

这些 case 在 override 中带有 `allow_destructive=true`，只有 runner pool 显式声明 `allow_destructive=true` 时才会被调度。

### 4. 报告内容

HTML 报告按 case 展示：

- suite 名称
- case ID
- target
- 平台
- runner pool
- 执行耗时
- 状态
- stdout/stderr 日志链接
- 已保存 PNG 列表和预览

## 目录说明

- `examples_test_utils.py`: 公共解析、匹配、渲染逻辑
- `generate_examples_manifest.py`: 生成 examples manifest
- `generate_ci_matrix.py`: 生成 CI 矩阵
- `run_examples_suite.py`: 执行 examples 并产出单 job 报告
- `merge_example_results.py`: 聚合多个 job 的报告
- `config/example_overrides.json`: 特殊交互/危险 case 配置
- `config/runner_pools.json`: CI runner 池定义

## 本地使用方式

生成 manifest：

```bash
cmake --build build --target ob_examples_tests_manifest
```

生成 CI matrix：

```bash
cmake --build build --target ob_examples_tests_matrix
```

执行部分 case：

```bash
python tests/examples_tests/run_examples_suite.py \
  --manifest build/tests/examples_tests/generated/examples_manifest.json \
  --build-root build \
  --case-ids beginner/depth_viewer;beginner/rgbd_viewer \
  --output-dir reports/examples/local
```

## 后续扩展建议

1. 如果后续希望进一步缩短 CI 时长，可以在 workflow 中按 matrix case target 列表做增量构建，而不是整包 examples 全量编译。
2. 如果 runner 侧已经维护了统一的设备资产台账，可以把 `runner_pools.json` 替换为生成式配置，而不是手写 JSON。
3. 如果需要更强的图像回归能力，可以在当前保存 PNG 基础上增加基线图差异对比。