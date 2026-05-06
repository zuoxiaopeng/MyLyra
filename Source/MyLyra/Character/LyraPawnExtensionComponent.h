#pragma once
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "LyraPawnExtensionComponent.generated.h"

#define UE_API MYLYRA_API

UCLASS()
class ULyraPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
GENERATED_BODY()
	
	
public:
	UE_API void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
	UE_API void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
	
protected:
	
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
};

#undef UE_API