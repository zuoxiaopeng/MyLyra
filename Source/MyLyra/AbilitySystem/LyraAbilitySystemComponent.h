#pragma once
#include "AbilitySystemComponent.h"
#include "LyraAbilitySystemComponent.generated.h"

#define UE_API MYLYRA_API

UCLASS(MinimalAPI)
class ULyraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	
	UE_API ULyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};

#undef UE_API