#pragma once
#include "ModularGameMode.h"
#include "LyraGameMode.generated.h"

#define UE_API MYLYRA_API


UCLASS(MinimalAPI, Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class ALyraGameMode : public AModularGameModeBase 
{
	GENERATED_BODY()
	
public:
	
};

#undef UE_API