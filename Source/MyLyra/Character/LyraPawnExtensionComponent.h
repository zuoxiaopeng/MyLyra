#pragma once
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "LyraPawnExtensionComponent.generated.h"

#define UE_API MYLYRA_API

class ULyraPawnData;
class ULyraAbilitySystemComponent;

UCLASS(MinimalAPI)
class ULyraPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
GENERATED_BODY()

public:
	
	UE_API ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);
	
	static UE_API const FName NAME_ActorFeatureName;
	
	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	UE_API virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	UE_API virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	UE_API virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	UE_API virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
	
	/** 返回指定 Actor 上挂载的 PawnExtensionComponent。 */
	UFUNCTION(BlueprintPure, Category = "Lyra|Pawn")
	static ULyraPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<ULyraPawnExtensionComponent>() : nullptr); }
	
	template<class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }
	
	UE_API void SetPawnData(const ULyraPawnData* InPawnData);
	
	UFUNCTION(BlueprintPure, Category = "Lyra|Pawn")
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	UE_API void InitializeAbilitySystem(ULyraAbilitySystemComponent* InASC, AActor* InOwnerActor);
	
	UE_API void UninitializeAbilitySystem();
	
	UE_API void HandleControllerChanged();
	
	UE_API void HandlePlayerStateReplicated();
	
	UE_API void SetupPlayerInputComponent();
	
	UE_API void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
	
	UE_API void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);
	
protected:
	
	UE_API virtual void OnRegister() override;
	UE_API virtual void BeginPlay() override;
	UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	UE_API void OnRep_PawnData();
	
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "Lyra|Pawn")
	TObjectPtr<const ULyraPawnData> PawnData;
	
	UPROPERTY(Transient)
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;
};

#undef UE_API