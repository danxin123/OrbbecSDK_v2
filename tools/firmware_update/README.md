# Firmware Update Tool

统一固件升级工具，合并了单设备升级和多设备批量升级功能。

## 用法

```bash
# 列出已连接设备
ob_firmware_update -l

# 升级单个设备（仅连接一台时自动选择）
ob_firmware_update -f firmware.bin

# 按序列号指定设备升级
ob_firmware_update -f firmware.bin -s CP1234567890

# 批量升级所有已连接设备
ob_firmware_update -f firmware.bin -a
```

## 参数

| 参数 | 说明 |
|------|------|
| `-h, --help` | 显示帮助信息 |
| `-l, --list` | 列出所有已连接设备 |
| `-f, --file <path>` | 固件文件路径 (.bin / .img) |
| `-s, --serial <sn>` | 按序列号指定目标设备 |
| `-a, --all` | 升级所有已连接设备 |

## 功能特性

- **单设备模式**: 完整的升级流程，支持 reboot-and-reupdate（部分设备需要两次写入）
- **批量模式** (`-a`): 依次升级所有设备，最后输出汇总报告（成功/不匹配/失败）
- 自动检测 ANSI 转义码支持，进度条就地刷新
- Linux 下自动使用 libuvc 后端以提高稳定性
- 升级完成后自动重启设备

## 注意事项

- GMSL 设备（如 Gemini335Lg）不支持热插拔
- 升级过程中请勿断开设备连接
- `-a` 和 `-s` 不可同时使用
