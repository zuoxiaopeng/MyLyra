#pragma once
#include "AbilitySystemComponent.h"
#include "LyraAbilitySystemComponent.generated.h"

#define UE_API MYLYRA_API

class ULyraAbilityTagRelationshipMapping;

UCLASS(MinimalAPI)
class ULyraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	
	UE_API ULyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UE_API void SetTagRelationshipMapping(ULyraAbilityTagRelationshipMapping* NewMapping);
	
	UE_API void ClearAbilityInput();
};

#undef UE_API