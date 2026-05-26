#pragma once

#include "NativeGameplayTags.h"

namespace LyraGameplayTags
{
	MYLYRA_API FGameplayTag FIndTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	MYLYRA_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	MYLYRA_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;
	
	MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
}
