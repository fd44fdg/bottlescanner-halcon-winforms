# BottleScanner WinForms (C++/CLI) 启动说明

这个目录提供了可直接复用的窗体代码：
- `Program.cpp`
- `MainForm.h`

## 1. 在 Visual Studio 创建项目

1. 新建项目：`CLR Empty Project (.NET Framework)`（C++）
2. 项目名建议：`BottleScannerWinFormsCli`
3. 平台：`x64`（必须和 HALCON、你的 native 代码一致）

## 2. 添加代码文件

1. 把本目录下 `Program.cpp`、`MainForm.h` 加入项目
2. 把 `../BottleScanner.cpp`、`../BottleScanner.h` 也加入同一个项目（或改成链接静态库）

## 3. 打开 WinForms 引用和 /clr

1. `项目属性 -> 常规 -> 公共语言运行时支持`：选择 `/clr`
2. `项目属性 -> 链接器 -> 系统 -> 子系统`：选择 `Windows (/SUBSYSTEM:WINDOWS)`
3. `项目属性 -> 配置属性 -> 高级 -> 目标文件扩展名`：默认 `.exe`
4. 添加 .NET 引用：`System`, `System.Windows.Forms`, `System.Drawing`

## 4. 配置 HALCON 头文件/库

1. `C/C++ -> 常规 -> 附加包含目录` 添加 HALCON include（例如 `C:\Program Files\MVTec\HALCON-24.11-Progress\include`）
2. `链接器 -> 常规 -> 附加库目录` 添加 HALCON lib（例如 `...\lib\x64-win64`）
3. `链接器 -> 输入 -> 附加依赖项` 添加 `halconcpp.lib`（按你的版本实际库名）

## 5. 运行时 DLL

确保 `halconcpp.dll`、`halcon.dll` 在可搜索路径中（任选其一）：
- 放到 exe 同目录
- 或将 HALCON `bin\x64-win64` 加到系统 `PATH`

## 6. 功能对应关系

- 左侧：HALCON 显示窗口（嵌入 `Panel`）
- 右侧：路径输入、浏览、`开始检测` 按钮、结果标签
- 点击 `开始检测`：
  - 调用 `BottleScanner::DoInspection`
  - 标签显示 `数量 + 耗时(ms)`
  - UI 叠加显示检测区域 (`HObject regions`)

