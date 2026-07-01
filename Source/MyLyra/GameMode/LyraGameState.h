#pragma once
#include "ModularGameState.h"
#include "LyraGameState.generated.h"

#define UE_API MYLYRA_API


UCLASS(MinimalAPI, Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class ALyraGameState : public AModularGameStateBase
{
	GENERATED_BODY()
	
public:
	
};

#undef UE_API