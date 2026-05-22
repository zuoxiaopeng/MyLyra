#include "LyraHealthComponent.h"

ULyraHealthComponent::ULyraHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
	
}

void ULyraHealthComponent::StartDeath()
{
}

void ULyraHealthComponent::FinishDeath()
{
}

void ULyraHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	
}
