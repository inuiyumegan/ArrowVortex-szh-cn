# ArrowVortex

> 一款开源的 **StepMania / osu!mania谱面编辑器**，支持 `.sm` / `.osu` 格式的谱面制作与编辑。

---

## 项目简介

**ArrowVortex** 是由 Bram "Fietsemaker" van de Wetering 最初开发的节奏游戏谱面编辑器，现已开源并以 GPLv3 许可证发布。它专为 **StepMania** 及其衍生节奏游戏（如 ITG、Etterna 等）设计，提供完整的谱面创建、编辑、转换和预览功能。

本仓库为 **ArrowVortex 汉化版**，提供完整的简体中文界面支持。

## 主要功能

| 模块 | 功能描述 |
|------|----------|
| **📝 谱面编辑** | 创建、编辑、删除谱面（Chart），支持多谱面管理 |
| **🎵 音符类型** | 支持 Steps（单键）、Mines（地雷）、Holds（长键）、Rolls（连键）、Fakes（假键）、Lifts（离键） |
| **🔄 音符转换** | 音符 ↔ 地雷 / 假键 / 离键；长键 ↔ 连键 / 单键；Routine ↔ ITG Couple |
| **📐 变换工具** | 镜像翻转（水平/垂直/双向）、扩展、压缩、量化（4分~192分） |
| **⏱ 节拍编辑** | 完整的 Tempo（BPM）编辑支持 |
| **🎧 音频支持** | 音频波形预览，BPM 分析辅助工具 |
| **📊 难度评估** | 内置 Rating 难度估计算法 |
| **🎮 自动演示** | Dancing Bot 自动演奏演示功能 |
| **🌐 多语言** | 原生支持英文，汉化版提供完整简体中文界面 |

## 目录结构

```
ArrowVortex-1.0.1/
├── src/                    # 源代码
│   ├── Core/               # 核心框架（渲染、字体、GUI、输入等）
│   ├── Dialogs/            # 对话框实现
│   ├── Editor/             # 谱面编辑器核心逻辑
│   ├── Managers/           # 管理器模块
│   ├── Simfile/            # Simfile 解析与处理
│   └── System/             # 系统相关工具
├── lib/                    # 第三方库
│   ├── freetype/           # 字体渲染
│   ├── libmad/             # MP3 解码
│   ├── libvorbis/          # OGG Vorbis 解码
│   └── lua/                # Lua 脚本引擎
├── build/                  # 构建配置
│   └── VisualStudio/       # Visual Studio 项目文件
│   ├── lang/               # 语言文件 (zh_CN.json)
│   ├── assets/             # 资源文件
│   ├── noteskins/          # 音符皮肤
│   └── settings/           # 配置文件
└── CREDITS                 # 致谢
```

## 构建指南

### Windows（推荐）

1. 安装 **Visual Studio 2022 Community**，勾选「使用 C++ 的桌面开发」工作负载
2. 确保安装了 **Windows 10 SDK**

## 第三方组件

| 组件 | 用途 | 许可证 |
|------|------|--------|
| FreeType | 字体渲染 | FTL / GPLv2 |
| libogg / libvorbis | OGG 音频解码 | BSD-style |
| libmad | MP3 音频解码 | GPLv2+ |
| Lua | 脚本引擎 | MIT |
| aubio | 音频分析 | GPLv3 |

## 致谢

- **Bram "Fietsemaker" van de Wetering** — 原作者
- [uvcat7](https://github.com/uvcat7) — 开源贡献
- [sukibaby](https://github.com/sukibaby) — 开源贡献
- [Psycast](https://github.com/Psycast) — 开源贡献
- [DeltaEpsilon7787](https://github.com/DeltaEpsilon7787) — 开源贡献
- [DolphinChips](https://github.com/DolphinChips) — 开源贡献
- [StarbotArc](https://github.com/StarbotArc) — 开源贡献
- 幽幽子的饲养员 — 简体中文汉化

## 许可证

本项目基于 **GPLv3**（或更高版本）许可证开源。详见 [LICENSE](LICENSE) 文件。

> Copyright © ArrowVortex Contributors.  
> 本软件按"原样"提供，不提供任何形式的明示或暗示担保。

---

上面是deepseek v4总结的，我懒得写了（

原项目https://github.com/uvcat7/ArrowVortex

因为每次翻译都需要硬编码，我懒，故不保证本项目更新速度以及会不会更新（