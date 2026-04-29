#pragma once

#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LyraCharacterMovementComponent.generated.h"

#define UE_API MYLYRA_API

MYLYRA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);


/*
 *  FLyraCharacterGroundInfo
 * 
 *  关于角色下方的地面信息，仅在需要时更新
 */
USTRUCT(BlueprintType)
struct FLyraCharacterGroundInfo
{
	GENERATED_BODY()
	
	FLyraCharacterGroundInfo() 
	: LastUpdateFrame(0)
	, GroundDistance(0.f)
	{}
	
	uint64 LastUpdateFrame;
	
	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;
	
	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/*
 * ULyraCharacterMovementComponent
 * 
 * 基础角色移动组件
 */
UCLASS(MinimalAPI, Config = Game)
class ULyraCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	
	UE_API ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	UE_API virtual void SimulateMovement(float DeltaTime) override;
	
	UE_API virtual bool CanAttemptJump() const override;
	
	// 返回当前的地面信息，如果信息已经过时了，调用其更新信息
	UFUNCTION(BlueprintCallable, Category = "Lyra|CharacterMovememt")
	UE_API const FLyraCharacterGroundInfo& GetGroundInfo();
	
	UE_API void SetReplicatedAcceleration(const FVector& InAcceleration);
	
	//~UMovementComponent interface
	UE_API virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	UE_API virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface
	
protected:
	
	UE_API virtual void InitializeComponent() override;
	
protected:
	
	// 角色地面信息缓存，仅通过 GetGroundInfo() 访问更新，不应直接读取
	FLyraCharacterGroundInfo CachedGroundInfo;
	
	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};

#undef UE_API