import { Actor, ActorComponent, AnimMontage, LyraHeroComponent, SceneComponent, TArray, uproperty } from 'ue';
import Ts_Default_Character from './Ts_Default_Character';

// 假设 BP_Hero_Default 的 C++ 导出名为 BP_Hero_Default_C
// 注意：在使用前，确保在 UE 中点击了 Puerts -> Generate d.ts
class Ts_Hero_Default extends Ts_Default_Character {
    @uproperty.uproperty(uproperty.EditAnywhere)
    DeathMontages!: TArray<AnimMontage>;

    // 示例1：添加一个通用的Actor组件
    @uproperty.uproperty(uproperty.EditAnywhere, uproperty.BlueprintReadOnly)
    MyActorComponent!: LyraHeroComponent;

    // 示例2：添加一个场景组件（可以设置位置、旋转等）
    @uproperty.uproperty(uproperty.EditAnywhere, uproperty.BlueprintReadOnly)
    MySceneComponent!: SceneComponent;

    // 示例3：添加一个自定义类型的组件（需要先有对应的C++类）
    // @uproperty.uproperty(uproperty.EditAnywhere)
    // MyCustomComponent!: CustomComponentType;

    // 构造函数
    constructor() {
        super();

        // 可以在构造函数中进行一些初始化
        // 注意：在Puerts中，组件的实际创建由Unreal Engine处理
        // 我们只需要声明属性，UE会在适当的时候创建并附加组件
    }

    // 初始化函数（如果需要）
    Initialize(): void {
        // 可以在这里对组件进行进一步的设置
        if (this.MySceneComponent) {
            // 设置场景组件的初始位置
            // 注意：在TypeScript中，可能需要使用不同的API
            // this.MySceneComponent.RelativeLocation = new Vector(0, 0, 100);
        }
    }

    OnDeathStarted(OwningActor: Actor): void {
        if (this.DeathMontages.Num() > 0) {
            const randomIndex = Math.floor(Math.random() * this.DeathMontages.Num());
            this.PlayAnimMontage(this.DeathMontages.Get(randomIndex), 1.0);
        }

        // 使用组件示例
        if (this.MyActorComponent) {
            // 可以调用组件的方法
            // this.MyActorComponent.Activate();
        }

        if (this.MySceneComponent) {
            // 可以获取或修改场景组件的位置
            // const currentLocation = this.MySceneComponent.RelativeLocation;
            // 进行一些操作...
        }
    }

}

export default Ts_Hero_Default;