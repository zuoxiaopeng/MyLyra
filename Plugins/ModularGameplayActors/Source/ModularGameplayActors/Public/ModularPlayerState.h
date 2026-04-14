#pragma once

#include "GameFramework/PlayerState.h"

#include "ModularPlayerState.generated.h"

#define UE_API MODULARGAMEPLAYACTORS_API


UCLASS(MinimalAPI, Blueprintable)
class AModularPlayerState : public APlayerState
{
	GENERATED_BODY()
	
};

