#pragma once
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Components/GameFrameworkComponent.h"
#include "LyraHealthComponent.generated.h"

#define UE_API MYLYRA_API

class ULyraHealthSet;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FlyraHealth_DeathEvent, AActor*, OwingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLyraHealth_AttributeChanged, ULyraHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigactor);


/*
 * ELyraDeathState
 * 
 * 定义当前死亡状态
 */
UENUM(BlueprintType)
enum class ELyraDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/*
 * ULyraHealthComponent
 * 
 * Actor组件，用于处理生命值相关相关
 */
UCLASS(MinimalAPI, Blueprintable, meta=(BlueprintSpawnableComponent))
class ULyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
	
public:
	
	UE_API ULyraHealthComponent(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintPure, Category = "Lyra|Health")
	static ULyraHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<ULyraHealthComponent>() : nullptr); }
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	UE_API void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC);
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	UE_API void UnInitializeFromAbilitySystem();
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	UE_API float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	UE_API float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	UE_API float GetHealthNormalized() const;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	ELyraDeathState GetDeathState() const { return DeathState; }
	
	// Meta = (ExpandBoolAsExecs = "ReturnValue") 用于生成蓝图bool引脚
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadDying() const { return (DeathState > ELyraDeathState::NotDead); }
	
	UE_API virtual void StartDeath();
	
	UE_API virtual void FinishDeath();
	
	UE_API virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);
	
public:
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FlyraHealth_DeathEvent OnDeathStarted;
	
	UPROPERTY(BlueprintAssignable)
	FlyraHealth_DeathEvent OnDeathFinished;
	
	
protected:

	UE_API virtual void OnUnregister() override;

	UE_API void ClearGameplayTags();

	UE_API virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	UE_API virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	UE_API virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	UE_API virtual void OnRep_DeathState(ELyraDeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const ULyraHealthSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	ELyraDeathState DeathState;
};

#undef UE_API