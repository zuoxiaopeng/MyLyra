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

---

## LyraCharacter 深度架构解析与独立实现指南

`LyraCharacter` 作为 Lyra 框架的核心角色基类，其核心设计哲学是**“轻量级数据与逻辑枢纽”**。它自身几乎不写具体的硬编码业务逻辑，而是作为一个“容器”将各个系统（GAS、输入、网络、生命周期）粘合在一起。

如果你希望脱离 Lyra 原生代码，在自己的项目中从零实现一个类似的高级多网络角色类，你需要理解并搭建以下**五大核心骨架**：

### 1. 接口与继承体系 (Interfaces & Inheritance)
一个合格的现代 UE5 角色类通常需要继承 `AModularCharacter`（支持 GameFeature 模块化动态注入组件），并实现以下核心接口：
- `IAbilitySystemInterface`：GAS 系统的基础，对外提供 `GetAbilitySystemComponent()`。
- `IGameplayCueInterface`：处理 GAS 相关的表现（特效、音效）。
- `IGameplayTagAssetInterface`：允许角色直接被 GameplayTag 查询。
- `ILyraTeamAgentInterface`：实现动态队伍分配与敌我识别。

### 2. 构造函数骨架 (Component Assembly)
在构造函数中，不要写任何业务逻辑，只做组件的装配和基础属性设置：
- **禁用 Tick**：`PrimaryActorTick.bCanEverTick = false;`，角色的更新应交由 MovementComponent 和其他专门的组件处理，角色自身 Tick 极度浪费性能。
- **自定义移动组件**：通过 `SetDefaultSubobjectClass<ULyraCharacterMovementComponent>` 替换默认的移动组件。
- **创建三大核心组件**：
  - `PawnExtensionComponent`：生命周期大管家。
  - `HealthComponent`：生命值与死亡逻辑接管。
  - `CameraComponent`：视角控制。

### 3. 多人网络生命周期骨架 (Lifecycle & Initialization)
**痛点**：在 UE 多人游戏中，客户端的 `Controller`、`PlayerState` 和 `Pawn` 的到达顺序是完全随机的。如果在错误的时机初始化 GAS，会导致各种空指针或状态不同步。
**Lyra 的解法**：角色类提供一组“感知方法”，将状态变化无脑汇报给 `PawnExtensionComponent`，由它来统筹，直到所有依赖就绪，再回调 `OnAbilitySystemInitialized`。

你需要重写以下骨架方法，并统统交由大管家处理：
```cpp
// 服务器端：附身与解除附身
virtual void PossessedBy(AController* NewController) override {
    Super::PossessedBy(NewController);
    PawnExtComponent->HandleControllerChanged(); // 骨架核心：通知大管家
}
virtual void UnPossessed() override;

// 客户端：控制器和 PlayerState 同步到达
virtual void OnRep_Controller() override {
    Super::OnRep_Controller();
    PawnExtComponent->HandleControllerChanged();
}
virtual void OnRep_PlayerState() override {
    Super::OnRep_PlayerState();
    PawnExtComponent->HandlePlayerStateReplicated();
}

// 最终的成功回调：在这里真正挂载 GAS 和 Health
virtual void OnAbilitySystemInitialized();
```

#### 3.1 委托 API 设计的门面模式 (Facade Pattern)
`PawnExtensionComponent` 没有直接暴露内部的 `FSimpleMulticastDelegate` 供外部 `Add/Remove`，而是提供了封装好的注册方法：
- `OnAbilitySystemInitialized_RegisterAndCall(FDelegate Delegate)`
- `OnAbilitySystemUninitialized_Register(FDelegate Delegate)`

这是典型的**门面模式 (Facade Pattern)** 应用，核心原因是：

1. **解决“晚绑定竞态”问题**
   如果直接暴露内部委托，晚绑定的组件（比如游戏逻辑里动态 `AddComponent` 的组件）会错过已经广播完的初始化事件。封装后，`_RegisterAndCall` 内部会自动检查：如果注册时 ASC 已经就绪，立刻原地 `Execute()` 一次回调，彻底杜绝时序问题。

   ```cpp
   // LyraPawnExtensionComponent.cpp 内部实现
   void ULyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
   {
       if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject())) {
           OnAbilitySystemInitialized.Add(Delegate);
       }
       
       // 追溯执行，解决晚绑定问题
       if (AbilitySystemComponent) {
           Delegate.Execute();
       }
   }
   ```

2. **`FDelegate` 是轻量级句柄**
   `FSimpleMulticastDelegate::FDelegate` 只是一个包含对象指针和成员函数指针的值类型，临时构建并传值完全没有性能问题，类似于传 `int`。

3. **封装内部状态管理**
   反初始化只允许“纯注册”，不能追溯执行，避免对象创建时立刻触发清理逻辑的灾难性问题。

### 4. GAS 与物理状态同步骨架 (State Synchronization)
角色的物理状态（如在空中、下蹲）需要实时映射为 GAS 的 `GameplayTag`，这样技能系统才能根据状态（例如“只能在地面释放”）进行拦截。
你需要重写以下方法：
```cpp
// 移动模式改变时（如落地、起跳），更新对应的 GameplayTag
virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

// 下蹲与起身时，赋予或移除 Status.Crouching Tag
virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
```

### 5. 极致的网络优化骨架 (Networking Black Magic)
这是 Lyra 区别于普通教程的高级部分，也是射击游戏手感的来源：
- **加速度压缩 (`PreReplication`)**：
  默认的 `FVector Acceleration` 占用 24 字节。你需要重写 `PreReplication`，利用 `FMath::CartesianToPolar` 将 X/Y 轴的加速度转换为角度和大小，再加上 Z 轴，量化为 3 个 `uint8`/`int8`（3 字节），节省 87% 的带宽。
- **快速共享同步 (`FastSharedReplication`)**：
  默认的属性同步（Property Sync）存在固定频率延迟。Lyra 会在服务器 `UpdateSharedReplication` 中，将位置、旋转、速度打包成 `FSharedRepMovement`，通过 `Unreliable NetMulticast RPC` 高频发送给客户端。客户端收到后直接覆盖当前状态并触发平滑插值，绕过属性同步。

### 6. 死亡流程骨架 (Death Flow)
角色不应该直接调用 `Destroy()`，而是通过事件驱动。
- `FellOutOfWorld`：掉出地图时，调用 `HealthComponent->DamageSelfDestruct()`。
- `OnDeathStarted`：血量归零时触发，禁用碰撞 (`SetCollisionEnabled(NoCollision)`) 和移动 (`DisableMovement()`)，播放死亡动画。
- `OnDeathFinished`：动画播放完毕或延迟后触发，调用 `DetachFromControllerPendingDestroy()` 并设置 `LifeSpan` 销毁 Actor。

---

### 📋 源码复刻（手抄）踩坑总结

在从 Lyra 原生代码迁移至自定义工程（如 `MyLyra`）的过程中，由于代码量庞大且部分逻辑耦合较深，容易出现以下致命的遗漏或笔误（均会导致崩溃或严重表现异常）：

| No. | 常见遗漏/错误点 | 导致后果与正确做法 |
|-----|-------------|------------|
| 1 | **委托绑定致命错误** | **表现**：在构造函数中，将反初始化委托误绑到了 `OnAbilitySystemInitialized` 上，导致角色销毁时重复调用初始化逻辑。<br>**正解**：`OnAbilitySystemUninitialized_Register` 必须绑定到 `OnAbilitySystemUnInitialized`。 |
| 2 | **`PossessedBy` 漏掉组件通知** | **表现**：服务器在附身时，扩展组件不知道 Controller 变了，从而卡死 ASC 的初始化流程。<br>**正解**：重写 `PossessedBy` 时必须调用 `PawnExtComponent->HandleControllerChanged();`。 |
| 3 | **`HealthComponent` 方法调用错误** | **表现**：调用引擎自带的 `InitializeComponent()` 会导致健康组件未正确挂载 ASC。<br>**正解**：必须调用 Lyra 专用的 `InitializeWithAbilitySystem(LyraASC)` 和 `UninitializeFromAbilitySystem()`。 |
| 4 | **死亡未真正禁用碰撞与移动** | **表现**：角色死后依然是一堵隐形的墙，且在斜坡上可能会继续滑动。<br>**正解**：在 `DisableMovementAndCollision` 中必须设置 `CapsuleComp->SetCollisionEnabled(NoCollision)` 并调用 `CharacterMovement->DisableMovement()`。 |
| 5 | **自定义移动 Tag 映射键值错误** | **表现**：自定义移动模式（如滑铲）的 Tag 无法正确同步给 ASC。<br>**正解**：在 `SetMovementModeTag` 的 `MOVE_Custom` 分支中，Map 查找的 Key 必须使用传入的 `CustomMovementMode` 参数，而非原生的 `MovementMode`。 |

> **提示**：在复刻复杂的 UE5 模块时，尤其是涉及 GAS 生命周期和网络同步的模块，强烈建议严格对照原生源码的函数调用时机和参数传递，切忌想当然地使用引擎的默认组件方法（如 `InitializeComponent`）。
