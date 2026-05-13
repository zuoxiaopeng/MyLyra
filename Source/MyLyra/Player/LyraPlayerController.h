// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LyraPlayerController.generated.h"


#define  UE_API MYLYRA_API

/**
 * 
 */
UCLASS(MinimalAPI, Config = Game, Meta  = (ShortTooltip = "The base player controller class used by this project."))
class ALyraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
	
	
};

#undef  UE_API