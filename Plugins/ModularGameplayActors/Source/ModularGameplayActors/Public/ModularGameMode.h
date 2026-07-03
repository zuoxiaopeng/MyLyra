// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "ModularGameMode.generated.h"


#define UE_API MODULARGAMEPLAYACTORS_API

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable)
class AModularGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
};

UCLASS(MinimalAPI, Blueprintable)
class AModularGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
};

#undef UE_API