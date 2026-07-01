#pragma once

#include "GameplayMessageProcessor.generated.h" 

#define UE_API MYLYRA_API

UCLASS(MinimalAPI, BlueprintType, meta = (BlueprintSpawnableComponent))
class UGameplayMessageProcessor : public UActorComponent
{
	GENERATED_BODY()

public:
	
};

#undef UE_API