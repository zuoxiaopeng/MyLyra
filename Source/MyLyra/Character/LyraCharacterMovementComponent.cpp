#include "LyraCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"	

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraCharacterMovementComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped");

namespace LyraCharacter
{
	static float GroundTraceDistance = 100000.0f; // 一千米
	FAutoConsoleVariable CVar_GroundTraceDistance(TEXT("LyraCharacter.GroundTraceDistance"), GroundTraceDistance, TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
};

ULyraCharacterMovementComponent::ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULyraCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	if (bHasReplicatedAcceleration)
	{
		const FVector OriginalAcceleration = Acceleration;
		Super::SimulateMovement(DeltaTime);
		Acceleration = OriginalAcceleration;
	}
	else
	{
		Super::SimulateMovement(DeltaTime);
	}
}

bool ULyraCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

// 返回角色地面信息，包括： 地面命中信息 HitResult， 以及角色离地距离 GroundDistance
const FLyraCharacterGroundInfo& ULyraCharacterMovementComponent::GetGroundInfo()
{
	// 未挂载角色或者为同次更新帧，直接放回
	if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
	{
		return CachedGroundInfo;
	}
	
	
	if (MovementMode == MOVE_Walking)
	{	
		// 行走状态下直接采用 CurrentFloor 的 HitResult
		CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
		CachedGroundInfo.GroundDistance = 0.0f;
	}
	else // 非行走状态下进行射线检测
	{
		const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
		check(CapsuleComp);
		
		const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
		// 射线以 Pawn 的碰撞通道查询
		const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
		// 射线起点：角色胶囊体中心
		const FVector TraceStart(GetActorLocation());
		// 射线终点：角色胶囊体中心高度减去预设射线长度以及胶囊体半高
		const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - LyraCharacter::GroundTraceDistance - CapsuleHalfHeight));
		
		// 构建碰撞查询参数
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ULyraCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
		FCollisionResponseParams ResponseParams;
		InitCollisionParams(QueryParams, ResponseParams);
		
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParams);
		
		CachedGroundInfo.GroundHitResult = HitResult;
		CachedGroundInfo.GroundDistance = LyraCharacter::GroundTraceDistance;
		
		// NavWalking 下 CurrentFloor 信息不可靠
		if (MovementMode == MOVE_NavWalking)
		{
			CachedGroundInfo.GroundDistance = 0.0f;
		}
		else if (HitResult.bBlockingHit)
		{
			CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
		}
	}
	
	CachedGroundInfo.LastUpdateFrame = GFrameCounter;
	
	return CachedGroundInfo;
}

void ULyraCharacterMovementComponent::SetReplicatedAcceleration(const FVector& InAcceleration)
{
	bHasReplicatedAcceleration = true;
	Acceleration = InAcceleration;
}

FRotator ULyraCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return FRotator(0, 0, 0);
		}
	}
	
	return Super::GetDeltaRotation(DeltaTime);
}

float ULyraCharacterMovementComponent::GetMaxSpeed() const
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
		{
			return 0;
		}
	}
	
	return Super::GetMaxSpeed();
}

void ULyraCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}
