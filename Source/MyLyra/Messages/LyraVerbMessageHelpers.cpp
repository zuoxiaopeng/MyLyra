#include "LyraVerbMessageHelpers.h"
#include "GameFramework/PlayerState.h"
#include "LyraVerbMessage.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraVerbMessageHelpers)

/////////////////////////////////////////
// FLyraVerbMessage

FString FLyraVerbMessage::ToString() const
{
	FString HumanReadableMessage;
	FLyraVerbMessage::StaticStruct()->ExportText(/*out*/ HumanReadableMessage, this, /*Default*/ nullptr, /*OwnerObject*/ nullptr, PPF_None, /*ExportRootScope*/ nullptr);
	return HumanReadableMessage;
}

/////////////////////////////////////////
//


APlayerState* ULyraVerbMessageHelpers::GetPlayerStateFromObject(UObject* Object)
{
	if (APlayerController* PC = Cast<APlayerController>(Object))
	{
		return PC->PlayerState;
	}
	
	if (APlayerState* TargetPS = Cast<APlayerState>(Object))
	{
		return TargetPS;
	}
	
	if (APawn* TargetPawn = Cast<APawn>(Object))
	{
		if (APlayerState* TargetPS = TargetPawn->GetPlayerState())
		return TargetPS;
	}
	
	return nullptr;
}
