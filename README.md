# MyLyra
Lyra Retype - 基于 Puerts (TypeScript) 扩展的 Lyra 框架项目

## Puerts 环境配置指南

本项目集成了腾讯开源的 Puerts 插件，允许使用 TypeScript/JavaScript 来编写和扩展虚幻引擎 (UE5) 游戏逻辑，尤其是结合 Lyra 框架进行高度模块化的业务开发。

以下是环境从零配置的核心步骤记录：

### 1. 插件与依赖安装
1. 将 Puerts 插件放入 `Plugins/Puerts` 目录下。
2. 下载对应系统与 UE 版本的 V8 引擎预编译库（如 `v8_8.4.371.19_...`），解压并放入 `Plugins/Puerts/ThirdParty/v8` 目录下。
3. 在项目根目录执行以下命令初始化 Node 环境并安装 TypeScript：
   ```bash
   npm init -y
   npm install typescript -D
   ```

### 2. TypeScript 编译配置 (`tsconfig.json`)
在项目根目录创建或修改 `tsconfig.json`，确保 `rootDir` 和 `outDir` 正确映射，并将类型声明文件指向 Puerts 提供的目录：
```json
{
  "compilerOptions": {
    "target": "esnext",
    "module": "nodenext",
    "experimentalDecorators": true,
    "sourceMap": true,
    "typeRoots": [
      "Typing",
      "./node_modules/@types"
    ],
    "outDir": "Content/JavaScript",
    "rootDir": "TypeScript"
  },
  "include": [
    "TypeScript/**/*"
  ]
}
```
*编译命令配置*：在 `package.json` 的 `scripts` 中添加 `"build": "tsc"`。

### 3. C++ 依赖与自动模式配置
为了让引擎自动管理 V8 虚拟机的生命周期，并支持热重载和蓝图生成，我们需要配置 C++ 模块和引擎的 INI 文件。

**1. 修改 `MyLyra.Build.cs`**
在 `PublicDependencyModuleNames` 中添加 `"Puerts"` 和 `"JsEnv"` 模块。

**2. 配置 `DefaultPuerts.ini`**
在 `Config/DefaultPuerts.ini` 中配置自动模式（Auto Mode），免去手动在 C++ (如 GameInstance) 中 new 虚拟机的麻烦：
```ini
[/Script/Puerts.PuertsSetting]
RootPath=Content/JavaScript
AutoModeEnable=True
DebugEnable=True
DebugPort=8080
WaitDebugger=False
NumberOfJsEnv=1
WatchDisable=False
```

### 4. 激活 Puerts 的“继承引擎类”与“生成蓝图”功能
在命令行中进入 `Plugins/Puerts` 目录，执行官方提供的环境初始化脚本：
```bash
node enable_puerts_module.js
```
*注意：此步骤会修改底层配置文件以解锁 TypeScript 直接生成 UE 蓝图资产的能力。执行完毕后，**必须重启 UE 编辑器并重新编译 C++ 项目**。*

### 5. 工作流演示：编写与生成 TS 蓝图
1. 在 `TypeScript` 目录下编写你的业务逻辑，继承 `UE.Object` 的子类。
2. 运行编译命令：`npm run build`。这会将 TS 编译为 JS 并输出到 `Content/JavaScript` 目录下。
3. 打开 UE 编辑器，在顶部工具栏点击 **Puerts -> Ts to Blueprint** (或 Generate Blueprint)。
4. 引擎会自动扫描 JS 文件，并在 `Content/Blueprints/TypeScript/` (默认路径) 生成对应的 `.uasset` 代理蓝图。
5. 此时即可将该蓝图作为普通的虚幻资产（如作为组件挂载，或在 Lyra 的 `PawnData` 中配置使用）。

> **关于验证警告**：生成蓝图时如果 Output Log 报出 `UpdateValidators request made before RegisterBlueprintValidators`，这是因为生成瞬间引擎验证器尚未就绪，属正常现象，不影响蓝图使用，手动打开编译一次即可消除。
