#include "LyraPawnExtensionComponent.h"

#include "LyraGameplayTags.h"
#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "LyraPawnData.h"
#include "GeometryCollection/GeometryCollectionParticlesData.h"
#include "Net/UnrealNetwork.h"


const FName ULyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULyraPawnExtensionComponent::ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 关闭tick
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
	
	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void ULyraPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULyraPawnExtensionComponent, PawnData);
}

bool ULyraPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);
	
	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == LyraGameplayTags::InitState_Spawned)
	{
		if (Pawn)
		{
			return true;
		}
	}
	
	if (CurrentState == LyraGameplayTags::InitState_Spawned && DesiredState == LyraGameplayTags::InitState_DataAvailable)
	{
		if (!Pawn)
		{
			return false;
		}
		
		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		
		if (bHasAuthority || bIsLocallyControlled)
		{
			if (!GetController<AController>())
			{
				return false;
			}
		}
		
		return true;
	}
	else if (CurrentState == LyraGameplayTags::InitState_DataAvailable && DesiredState == LyraGameplayTags::InitState_DataInitialized)
	{
		return Manager->HaveAllFeaturesReachedInitState(Pawn, LyraGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == LyraGameplayTags::InitState_DataInitialized && DesiredState == LyraGameplayTags::InitState_GameplayReady)
	{
		return true;
	}
	
	return false;
}

void ULyraPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == LyraGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void ULyraPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == LyraGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

void ULyraPawnExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();
	
	static const TArray<FGameplayTag> StateChain = { LyraGameplayTags::InitState_Spawned, LyraGameplayTags::InitState_DataAvailable, LyraGameplayTags::InitState_DataInitialized, LyraGameplayTags::InitState_GameplayReady };
	
	ContinueInitStateChain(StateChain);
}

void ULyraPawnExtensionComponent::SetPawnData(const ULyraPawnData* InPawnData)
{
	check(InPawnData);
	
	APawn* Pawn = GetPawnChecked<APawn>();
	
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	
	if (Pawn)
	{
		UE_LOG(LogLyra, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}
	
	PawnData = InPawnData;
	
	Pawn->ForceNetUpdate();
	
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::InitializeAbilitySystem(ULyraAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);
	
	if (AbilitySystemComponent == InASC)
	{
		return;
	}
	
	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}
	
	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();
	
	UE_LOG(LogLyra, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC),  *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));
	
	if ((ExistingAvatar != nullptr) && ExistingAvatar != Pawn)
	{
		UE_LOG(LogLyra, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);
		
		ensure(!ExistingAvatar->HasAuthority());
		
		if (ULyraPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}
	
	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
	
	if (ensure(PawnData))
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}
	
	OnAbilitySystemInitialized.Broadcast();
}

void ULyraPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(LyraGameplayTags::Ability_Behavior_SurvivesDeath);
		
		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();
		
		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}
		
		OnAbilitySystemUninitialized.Broadcast();
	}
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
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
	
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void ULyraPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

void ULyraPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();
	
	const APawn* Pawn = GetPawnChecked<APawn>();
	// 确认组件挂载在 Pawn 上
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("LyraPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));
	
	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(ULyraPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	// 一个 Pawn 上只允许有一个 PawnExtensionComponent
	ensureAlwaysMsgf(PawnExtensionComponents.Num() == 1, TEXT("Only one LyraPawnExtensionComponet should exist on [%s]."), *GetNameSafe(GetOwner()));
	
	// 注册进 GameFrameworkComponentManager 的初始化状态系统，应尽早注册，仅在 Game World 生效（!ReceiverWorld->IsGameWorld() || ReceiverWorld->IsPreviewWorld()）
	RegisterInitStateFeature();
}

void ULyraPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	ensure(TryToChangeInitState(LyraGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void ULyraPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}
