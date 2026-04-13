// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ALyraCharacter::ALyraCharacter(const FObjectInitializer& ObjectInitializer)
{
 	
}

UAbilitySystemComponent* ALyraCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ALyraCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	
}

void ALyraCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	ILyraTeamAgentInterface::SetGenericTeamId(NewTeamID);
}

FGenericTeamId ALyraCharacter::GetGenericTeamId() const
{
	return ILyraTeamAgentInterface::GetGenericTeamId();
}

FOnLyraTeamIndexChangedDelegate* ALyraCharacter::GetOnTeamIndexChangedDelegate()
{
	return ILyraTeamAgentInterface::GetOnTeamIndexChangedDelegate();
}
