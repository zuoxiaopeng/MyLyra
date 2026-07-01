#pragma once

#include "CommonGameInstance.h"
#include "LyraGameInstance.generated.h"

#define UE_API MYLYRA_API

class UObject;

UCLASS(MinimalAPI, Config = Game)
class ULyraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

};

#undef UE_API