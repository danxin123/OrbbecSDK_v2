# OpenNI to UVC Upgrade Guide

Starting from October 2025 (Orbbec SDK v2.5.5), Orbbec is upgrading OpenNI-based devices to the UVC protocol to fully support Orbbec SDK v2.

## Upgrade Timeline and Scope

| Already Upgraded Devices | Planned Upgrade Devices |
|---|---|
| Astra Mini S Pro, Astra Mini Pro | Gemini E, Gemini UW, Gemini EW, DaBai Max, DaBai Max Pro, DaBai DW, DaBai DCW, DaBai DCW2, DaBai DW2 |

## Firmware Upgrade Tool

Use the official firmware upgrade repository:

- [OrbbecFirmware](https://github.com/orbbec/OrbbecFirmware)

## Upgrade Workflow

1. Download the firmware package for your exact device model.
2. Upgrade firmware with the official upgrade tool.
3. Switch to Orbbec SDK v2 APIs after upgrade.

Detailed migration and API usage instructions:

- [opennisdk_to_orbbecsdkv2.md](opennisdk_to_orbbecsdkv2.md)

## Important Notes

- Firmware major version `2.x.x` indicates UVC protocol and requires Orbbec SDK v2.
- Firmware major version `1.x.x` indicates OpenNI protocol and requires OpenNI SDK or Orbbec SDK v1.
