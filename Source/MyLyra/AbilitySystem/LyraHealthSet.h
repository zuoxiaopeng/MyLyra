#pragma once

#include "AbilitySystemComponent.h"
#include "LyraAttributeSet.h"
#include "NativeGameplayTags.h"
#include "LyraHealthSet.generated.h"

#define UE_API MYLYRA_API

// C++ Native tag 定义方式
// 声明Tag
MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Lyra_Damage_Message);

struct FGameplayEffectModCallbackData;

UCLASS(MinimalAPI, BlueprintType)
class ULyraHealthSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	
	UE_API ULyraHealthSet();
	
	ATTRIBUTE_ACCESSORS_BASIC(ULyraHealthSet, Health);
	ATTRIBUTE_ACCESSORS_BASIC(ULyraHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS_BASIC(ULyraHealthSet, Healing);
	ATTRIBUTE_ACCESSORS_BASIC(ULyraHealthSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FLyraAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FLyraAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FLyraAttributeEvent OnOutOfHealth;
	
	
protected:
	UFUNCTION()
	UE_API void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	UE_API void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	UE_API virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	UE_API virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UE_API virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	UE_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	UE_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	UE_API void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
	
private:
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Lyra|Helath", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	
	bool bOutOfHealth;
	
	// 保存变化前的血量信息
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;
	
	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	//	Meta: 临时属性，如 治疗量 伤害量
	//  Stateful: 持久属性，如 生命 法力 
	// -------------------------------------------------------------------
	
	UPROPERTY(BlueprintReadOnly, Category="Lyra|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;
	
	UPROPERTY(BlueprintReadOnly, Category="Lyra|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};

#undef UE_API