# ThreeFingerDrag 安装与配置

下载 Release 中的 `ThreeFingerDrag.Setup.1.3.0.exe`，运行安装向导并选择安装目录。
默认仅为当前用户安装，也可选择为所有用户安装（需要管理员权限）。
可选创建桌面快捷方式和“登录 Windows 时启动”。

安装目录包含 EXE、`config.ini`、`log.txt`、`version.txt`、许可证和卸载程序。
配置固定从 EXE 所在目录读取，与当前工作目录无关，支持中文目录。
首次安装默认取消延迟 1500 毫秒、手势速度 45。升级保留已有配置，不覆盖个人设置。
托盘 Settings 修改保存回该配置；手动编辑后退出并重启程序生效。
安装器只为配置和日志文件授予普通用户写权限。

开机启动通过当前用户的
`HKCU\Software\Microsoft\Windows\CurrentVersion\Run\ThreeFingerDrag`
保存带引号的完整 EXE 路径，在用户登录后执行。托盘 Settings 可随时开关。
旧版当前用户的 ThreeFingerDrag 计划任务在运行时迁移。
卸载清理指向此安装路径的当前用户启动项；保留个人配置和日志。

运行新版前先从托盘退出旧版。需要 Windows 10/11 x64 精密触摸板。

## 构建

需要 Visual Studio C++ 桌面开发工具、Windows SDK 和 Inno Setup 6.7+。
将 ISCC.exe 所在目录加入 PATH（或设置 ISCC 环境变量为编译器完整路径），运行 `build-setup.cmd`。
脚本以 Release/x64 和静态运行库 `/MT` 构建，再编译安装包。
安装包生成在 `installers` 目录，不需要另装 Visual C++ Redistributable。
