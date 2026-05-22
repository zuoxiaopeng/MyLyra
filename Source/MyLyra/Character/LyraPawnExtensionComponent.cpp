#include "LyraPawnExtensionComponent.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"

ULyraPawnExtensionComponent::ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
{
}

void ULyraPawnExtensionComponent::InitializeAbilitySystem()
{
}

void ULyraPawnExtensionComponent::UninitializeAbilitySystem()
{
}

void ULyraPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		} 
		else
		{
			InitializeAbilitySystem();
		}
	}
}

void ULyraPawnExtensionComponent::HandlePlayerStateReplicated()
{
	
}

void ULyraPawnExtensionComponent::SetupPlayerInputComponent()
{
	
}

void ULyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
}

void ULyraPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
}
