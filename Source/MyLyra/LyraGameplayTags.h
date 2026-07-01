#pragma once

#include "NativeGameplayTags.h"

namespace LyraGameplayTags
{
	MYLYRA_API FGameplayTag FIndTagByString(const FString& TagString, bool bMatchPartialString = false);
	
	// Declare all of the custom native tags that Lyra will use
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_GodMode);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cheat_UnlimitedHealth);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	MYLYRA_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	MYLYRA_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	MYLYRA_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
	
}
