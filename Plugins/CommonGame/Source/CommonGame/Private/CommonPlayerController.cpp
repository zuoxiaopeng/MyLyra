#include "CommonPlayerController.h"

ACommonPlayerController::ACommonPlayerController(const FObjectInitializer& ObjectInitializer)
{
}

void ACommonPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ACommonPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
}

void ACommonPlayerController::OnPossess(class APawn* APawn)
{
	Super::OnPossess(APawn);
}

void ACommonPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACommonPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}
