#pragma once

#include "LyraGameData.generated.h"

#define UE_API MYLYRA_API

class UGameplayEffect;

UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Lyra Game Data", ShortTooltip = "Data asset containing global game data."))
class ULyraGameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UE_API ULyraGameData();
	
	static UE_API const ULyraGameData& Get();
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Defaut Gameplay Effects", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
	TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;
	
};

#undef UE_API