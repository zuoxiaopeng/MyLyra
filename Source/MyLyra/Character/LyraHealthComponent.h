#pragma once
#include "Components/GameFrameworkComponent.h"
#include "LyraHealthComponent.generated.h"

#define UE_API MYLYRA_API

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FlyraHealth_DeathEvent, AActor*, OwingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLyraHealth_AttributeChanged, ULyraCameraComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigactor);

UCLASS(MinimalAPI, Blueprintable, meta=(BlueprintSpawnableComponent))
class ULyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
	
public:
	UE_API ULyraHealthComponent(const FObjectInitializer& ObjectInitializer);
	//TODO: 补全内容
	
public:
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FlyraHealth_DeathEvent OnDeathStarted;
	
	UPROPERTY(BlueprintAssignable)
	FlyraHealth_DeathEvent OnDeathFinished;
};

#undef UE_API
