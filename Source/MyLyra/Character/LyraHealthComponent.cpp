#include "LyraHealthComponent.h"

#include "LyraGameplayTags.h"
#include "LyraLogChannels.h"
#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/LyraHealthSet.h"
#include "Chaos/Deformable/Utilities.h"
#include "Messages/LyraVerbMessage.h"
#include "Messages/LyraVerbMessageHelpers.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraHealthComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Lyra_Elimination_Message, "Lyra.Elimination.Message");

ULyraHealthComponent::ULyraHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) // 采用FObjectInitializer构造函数，用于编辑父类Subobjects
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
	
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = ELyraDeathState::NotDead;
}

void ULyraHealthComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	// 获取组件拥有者，确保组件挂载在一个Actor上（通常是Character）
	AActor* Owner = GetOwner();
	check(Owner);
	
	// 检查是否已初始化，防止重复调用初始化函数
	if (AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}
	
	// 绑定 ASC
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}
	
	// 获取属性集合
	// 组件本身不做数据存储
	HealthSet = AbilitySystemComponent->GetSet<ULyraHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}
	
	// 绑定委托（事件监听）
	// 监听Set中的属性变化，触发组件中的回调
	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	
	// 初始化当前血量
	// Temp：临时方案，重置属性，最终将由数据表格驱动
	AbilitySystemComponent->SetNumericAttributeBase(ULyraHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	
	// 状态清理
	ClearGameplayTags();
	
	// 主动广播血量变化
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void ULyraHealthComponent::UnInitializeFromAbilitySystem()
{
	// 状态清理
	ClearGameplayTags();
	
	// 移除委托
	if (HealthSet)
	{
		HealthSet->OnHealthChanged.RemoveAll(this);
		HealthSet->OnMaxHealthChanged.RemoveAll(this);
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}
	
	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float ULyraHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float ULyraHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float ULyraHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		
		return ((MaxHealth > 0) ? (Health / MaxHealth) : 0.0f);
	}
	
	return 0.0f;
}

void ULyraHealthComponent::StartDeath()
{
	if (DeathState != ELyraDeathState::NotDead)
	{
		return;
	}
	
	DeathState = ELyraDeathState::DeathStarted;
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dying, 1);
	}
}

void ULyraHealthComponent::FinishDeath()
{
	if (DeathState != ELyraDeathState::DeathStarted)
	{
		return;
	}
	
	DeathState = ELyraDeathState::DeathFinished;
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dead, 1);
	}
	
	AActor* Owner = GetOwner();
	check(Owner);
	
	OnDeathFinished.Broadcast(Owner);
	
	Owner->ForceNetUpdate();
}

// 自毁
void ULyraHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState == ELyraDeathState::NotDead) && AbilitySystemComponent)
	{
		const TSubclassOf<UGameplayEffect> DamageGE = ULyraAssetManager::GetSubClass(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
		if (!DamageGE)
		{
			UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to find gameplay effect [%s]."), *GetNameSafe(GetOwner()), *ULyraGameData::Get().DamageGameplayEffect_SetByCaller.GetAssetName());
			return;
		}
		
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1.0, AbilitySystemComponent->MakeEffectContext());
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
		
		if (!Spec)
		{
			UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: DamageSelfDestruct failed for owner [%s]. Unable to make outgoing spec for [%s]"), *GetNameSafe(GetOwner()), *GetNameSafe(DamageGE));
			return;
		}
		
		Spec->AddDynamicAssetTag(TAG_Gameplay_DamageSelfDestruct);
		
		if (bFellOutOfWorld)
		{
			Spec->AddDynamicAssetTag(TAG_Gameplay_FellOutOfWorld);
		}
		
		const float DamageAmount = GetMaxHealth();
		
		Spec->SetSetByCallerMagnitude(LyraGameplayTags::SetByCaller_Damage, DamageAmount);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
	}
}

void ULyraHealthComponent::OnUnregister()
{
	UnInitializeFromAbilitySystem();

	Super::OnUnregister();
}

void ULyraHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dead, 0);
	}
}

void ULyraHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void ULyraHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void ULyraHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
	float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		// 大括号用于限制局部变量作用域
		{
			
			// 死亡事件包
			FGameplayEventData Payload;
			Payload.EventTag = LyraGameplayTags::GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;
			
			/*
			 * 生成一个 PredictionKey
			 * Prediction： 客户端正常跑逻辑，不等待服务器验证，服务器判断是否认可预测结果，不认可则回滚
			 * 开启预测窗口
			 */
			FScopedPredictionWindow NewScopeWindow(AbilitySystemComponent, true);
			// 执行 gameplay event 处于预测上下文内
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}
		
		{
			/*
			 * 击杀消息
			 * 提供外部订阅
			 */
			FLyraVerbMessage Message;
			Message.Verb = TAG_Lyra_Elimination_Message;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Message.Target = ULyraVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			
			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}
	}
#endif
}

// 服务器修改并复制DeathState时触发
void ULyraHealthComponent::OnRep_DeathState(ELyraDeathState OldDeathState)
{
	const ELyraDeathState NewDeathState = DeathState;
	
	// 先回退死亡状态，保证死亡流程顺序进行
	DeathState = OldDeathState;
	
	// 客户端预测跑的比服务器更快，保留预测状态
	if (OldDeathState > NewDeathState)
	{
		UE_LOG(LogLyra, Warning, TEXT("LyraHelathComponent: Predicted past server death state [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		return;
	}
	
	if (OldDeathState == ELyraDeathState::NotDead)
	{
		if (NewDeathState == ELyraDeathState::DeathStarted)
		{
			StartDeath();
		}
		else if (NewDeathState == ELyraDeathState::DeathFinished)
		{
			StartDeath();
			FinishDeath();
		}
		else
		{
			UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	else if (OldDeathState == ELyraDeathState::DeathStarted)
	{
		if (NewDeathState == ELyraDeathState::DeathFinished)
		{
			FinishDeath();
		}
		else
		{
			UE_LOG(LogLyra, Error, TEXT("LyraHealthComponent: Invalid death transition [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
		}
	}
	
	ensureMsgf((OldDeathState == NewDeathState), TEXT("LyraHealthComponent: Death transition failed [%d] -> [%d] for owner [%s]."), (uint8)OldDeathState, (uint8)NewDeathState, *GetNameSafe(GetOwner()));
}
