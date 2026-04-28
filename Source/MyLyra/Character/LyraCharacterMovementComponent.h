#pragma once
#include "GameFramework/CharacterMovementComponent.h"

#define UE_API MYLYRA_API

UCLASS(MinimalAPI, Config = Game)
class ULyraCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	
	UE_API ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
};


#undef UE_API