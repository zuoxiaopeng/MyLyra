#include "LyraCharacterMovementComponent.h"

ULyraCharacterMovementComponent::ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void ULyraCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	Super::SimulateMovement(DeltaTime);
}

bool ULyraCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump();
}

const FLyraCharacterGroundInfo& ULyraCharacterMovementComponent::GetGroundInfo()
{
	return CachedGroundInfo;
}

void ULyraCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
}

FRotator ULyraCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	return Super::GetDeltaRotation(DeltaTime);
}

float ULyraCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed();
}

void ULyraCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
