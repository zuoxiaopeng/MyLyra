#pragma once

#include "Engine/DataAsset.h"

#include "LyraPawnData.generated.h"

#define UE_API MYLYRA_API

class ULyraAbilityTagRelationshipMapping;

UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Lyra Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class ULyraPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	
	UE_API ULyraPawnData(const FObjectInitializer& ObjectInitializer);
	
	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};

#undef UE_API