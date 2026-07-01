#pragma once
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "LyraHeroComponent.Generated.h"

#define UE_API MYLYRA_API

UCLASS(MinimalAPI, Blueprintable, Meta = (BlueprintSpawnableComponent))
class ULyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	
};

#undef UE_API