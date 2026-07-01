#pragma once

#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayMessageSubsystem.generated.h"

#define UE_API GAMEPLAYMESSAGERUNTIME_API


UCLASS(MinimalAPI)
class UGameplayMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	
	static UE_API UGameplayMessageSubsystem& Get(const UObject* WorldContextObject);
	
	template <typename  FMessageStructType>
	void BroadcastMessage(FGameplayTag Channel, const FMessageStructType& Message)
	{
		const UScriptStruct* StructType = TBaseStructure<FMessageStructType>::Get();
	}
};
#undef UE_API
