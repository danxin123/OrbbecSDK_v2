# obsensor_metadata_win10

This document introduces the process for getting metadata from cameras with UVC protocal on Windows system. The following process has been verified on Windows 10 and Windows 11.

## One-Click Setup

Use the wrapper script below for the default installation flow:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
powershell -ExecutionPolicy Bypass -File .\scripts\env_setup\setup.ps1
```

To install metadata keys for all previously connected Orbbec devices explicitly:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\env_setup\setup.ps1 -Operation install_all
```

## Registering Metadata Using the Low-Level Script

To get device metadata through UVC protocol, users need to modify the registry and complete registration first, because Windows system has default limitations.

1. Connect the device and confirm that the device is online;
2. Open PowerShell with administrator privileges, then use the `cd` command to enter the directory where the script is located;
3. Execute the `Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser` command, and enter `Y` as prompted to confirm;
   - Try to execute the `Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope CurrentUser` command if the previous command fails in some cases;
4. Execute `.\obsensor_metadata_win10.ps1 -op install_all` to complete the registration.

**Notes**:
Users need to run this script every time a new device is connected. This process can be cumbersome and easy to forget, leading to an inability to obtain the device's metadata.

## Ready-to-Paste Commands

Install for currently connected devices:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
cd .\scripts\env_setup
.\obsensor_metadata_win10.ps1 -op install
```

Install for all previously connected devices:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
cd .\scripts\env_setup
.\obsensor_metadata_win10.ps1 -op install_all
```
