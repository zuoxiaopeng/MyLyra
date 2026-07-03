// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/GameStateBase.h"
#include "ModularGameState.generated.h"


#define UE_API MODULARGAMEPLAYACTORS_API

/**
 * 
 */
UCLASS(MinimalAPI, Blueprintable)
class AModularGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
};

UCLASS(MinimalAPI, Blueprintable)
class AModularGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
};

#undef UE_API