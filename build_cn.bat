@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================================
echo   ArrowVortex 汉化版 - 一键构建脚本
echo ============================================
echo.

REM 设置路径
set "PROJECT_DIR=%~dp0"
set "SOLUTION=%PROJECT_DIR%build\VisualStudio\ArrowVortex.sln"
set "OUTPUT_DIR=%PROJECT_DIR%bin"
set "RELEASE_DIR=%PROJECT_DIR%ArrowVortex_CN"

REM 1. 检查 Visual Studio
echo [1/4] 检查 Visual Studio...
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [错误] 未找到 Visual Studio，请先安装 VS 2022 Community
    pause
    exit /b 1
)
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -property installationPath`) do set "VS_PATH=%%i"
echo   VS 路径: %VS_PATH%

if not exist "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" (
    echo [错误] 未找到 vcvars64.bat，请确认安装了「使用 C++ 的桌面开发」
    pause
    exit /b 1
)

REM 2. 检查 Windows SDK
echo [2/4] 检查 Windows SDK...
set "SDK_FOUND=0"
for /d %%d in ("%ProgramFiles(x86)%\Windows Kits\10\Include\10.*") do (
    echo   找到 SDK: %%~nxd
    set "SDK_FOUND=1"
)
if "%SDK_FOUND%"=="0" (
    echo.
    echo [警告] 未找到 Windows SDK！ 
    echo 请手动执行以下步骤：
    echo   1. 打开 Visual Studio Installer
    echo   2. 点击「修改」
    echo   3. 勾选「Windows 10 SDK」(在「使用 C++ 的桌面开发」中)
    echo   4. 点击「修改」安装
    echo.
    echo 或者以管理员身份运行以下命令：
    echo   "C:\Program Files (x86)\Microsoft Visual Studio\Installer\setup.exe" modify --installPath "%VS_PATH%" --add Microsoft.VisualStudio.Component.Windows10SDK.19041 --quiet
    echo.
    pause
    exit /b 1
)

REM 3. 编译项目
echo [3/4] 开始编译...
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
msbuild "%SOLUTION%" /p:Configuration="release" /p:Platform="x64" /t:Build /v:minimal /m
if errorlevel 1 (
    echo.
    echo [错误] 编译失败！请检查上方错误信息。
    pause
    exit /b 1
)
echo   编译成功！

REM 4. 打包为可运行目录
echo [4/4] 打包...
if exist "%RELEASE_DIR%" rmdir /s /q "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%"
mkdir "%RELEASE_DIR%\assets"
mkdir "%RELEASE_DIR%\lang"
mkdir "%RELEASE_DIR%\noteskins"
mkdir "%RELEASE_DIR%\settings"

REM 复制可执行文件 (所有库静态链接，无需 DLL)
xcopy /y "%OUTPUT_DIR%\ArrowVortex.exe" "%RELEASE_DIR%\" >nul 2>&1
if not exist "%RELEASE_DIR%\ArrowVortex.exe" (
    echo [错误] 未找到 ArrowVortex.exe，编译可能未成功
    pause
    exit /b 1
)

REM 复制资源文件
xcopy /y /e "%OUTPUT_DIR%\assets\*" "%RELEASE_DIR%\assets\" >nul 2>&1
xcopy /y /e "%OUTPUT_DIR%\lang\*" "%RELEASE_DIR%\lang\" >nul 2>&1
xcopy /y /e "%OUTPUT_DIR%\noteskins\*" "%RELEASE_DIR%\noteskins\" >nul 2>&1
xcopy /y /e "%OUTPUT_DIR%\settings\*" "%RELEASE_DIR%\settings\" >nul 2>&1

REM 复制许可文件
xcopy /y "%PROJECT_DIR%CREDITS" "%RELEASE_DIR%\" >nul 2>&1
xcopy /y "%PROJECT_DIR%LICENSE" "%RELEASE_DIR%\" >nul 2>&1
xcopy /y "%PROJECT_DIR%README.md" "%RELEASE_DIR%\" >nul 2>&1

echo.
echo ============================================
echo   打包完成！
echo   输出目录: %RELEASE_DIR%
echo   可执行文件: %RELEASE_DIR%\ArrowVortex.exe
echo ============================================
echo.

REM 询问是否创建 ZIP
set /p "CREATE_ZIP=是否创建 ZIP 压缩包？(Y/N): "
if /i "%CREATE_ZIP%"=="Y" (
    powershell -Command "Compress-Archive -Path '%RELEASE_DIR%' -DestinationPath '%PROJECT_DIR%ArrowVortex_CN.zip' -Force"
    echo   ZIP 已创建: %PROJECT_DIR%ArrowVortex_CN.zip
)

pause
