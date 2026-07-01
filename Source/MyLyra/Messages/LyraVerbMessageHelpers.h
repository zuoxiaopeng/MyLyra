#pragma once

#include "LyraVerbMessageHelpers.generated.h"

#define UE_API MYLYRA_API

UCLASS(MinimalAPI)
class ULyraVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lyra")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);
	
};


#undef UE_API 