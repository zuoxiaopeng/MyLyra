#include "LyraCharacter.h"

#include "LyraCharacterMovementComponent.h"
#include "LyraHealthComponent.h"
#include "LyraLogChannels.h"
#include "LyraPawnExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraPlayerState.h"
#include "Player/LyraPlayerController.h"
#include "System/LyraSignificanceManager.h"

class ULyraCharacterMovementComponent;

static FName NAME_LyraCharacterCollisionProfile_Capsule(TEXT("LyraPawnCapsule"));
static FName NAME_LyraCharacterCollisionProfile_Mesh(TEXT("LyraPawnMesh"));

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
		
		// 大世界中离世界原点过远时，浮点数精度会下降(float，导致物理抖动以及渲染破面
		// 为此引入世界原点平移（World Origin Rebasing）
		// 服务器使用FRepMovement::RebaseOntoZeroOrigin统一原点
		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->GetProxyIsJumpForceApplied() || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;
		
		// 未开启线性平滑则将时间戳置零
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		} else
		{
			RepTimeStamp = 0.f;
		}
		
		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}
	
	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}
	
	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}
	
	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}
	
	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}
	
	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}
	
	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	
	// 先序列化原生同步数据（FIFO，先进先出）
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;
	
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	// 压缩为 1 bit，兼具收发功能，需要地址参数
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	} else
	{
		// 发包无行为，收包时本地时间戳置0
		RepTimeStamp = 0.f;
	}
	
	return true;
}

ALyraCharacter::ALyraCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<ULyraCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 关闭 Tick， 通过组件 Tick 满足更新需要， Character 本身关闭 Tick 节省性能
	PrimaryActorTick.bCanEverTick = false;
	// Tick 初始状态为关闭
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SetNetCullDistanceSquared(900000000.0f);
	
	UCapsuleComponent* CapsuleCom = GetCapsuleComponent();
	check(CapsuleCom);
	CapsuleCom->InitCapsuleSize(40.f, 90.f);
	CapsuleCom->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Capsule);
	
	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // 左手坐标系，Mesh导入时默认 Y 轴朝向，绕 Z 逆时针旋转 90 度，设为 X 轴朝向
	MeshComp->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Mesh);
	
	ULyraCharacterMovementComponent* LyraMoveComp = CastChecked<ULyraCharacterMovementComponent>(GetCharacterMovement());
	LyraMoveComp->GravityScale = 1.0f;
	LyraMoveComp->MaxAcceleration = 2400.0f;
	LyraMoveComp->BrakingFrictionFactor = 1.0f;
	LyraMoveComp->BrakingFriction = 6.0f;
	LyraMoveComp->GroundFriction = 8.0f;
	LyraMoveComp->BrakingDecelerationWalking = 1400.0f;
	// 角色朝向跟随视角，平滑转动
	LyraMoveComp->bUseControllerDesiredRotation = false;
	// 角色朝向跟随移动方向
	LyraMoveComp->bOrientRotationToMovement = false;
	LyraMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	// 动画期间关闭物理旋转
	LyraMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	// 控制下蹲功能开关，可以给 bot 使用
	LyraMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	// 下蹲时是否允许掉下悬崖/边缘
	LyraMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	// 蹲下时角色胶囊体半高
	LyraMoveComp->SetCrouchedHalfHeight(65.0f);
	
	PawnExtComponent = CreateDefaultSubobject<ULyraPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	
	HealthComponent = CreateDefaultSubobject<ULyraHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
	
	CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

ALyraPlayerController* ALyraCharacter::GetLyraPlayerController() const
{
	return Cast<ALyraPlayerController>(GetController());
}

ALyraPlayerState* ALyraCharacter::GetLyraPlayerState() const
{
	return CastChecked<ALyraPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

ULyraAbilitySystemComponent* ALyraCharacter::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ALyraCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}
	
	return PawnExtComponent->GetLyraAbilitySystemComponent();
}

void ALyraCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		LyraASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ALyraCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasMatchingGameplayTag(TagToCheck);
	}
	
	return false;
}

bool ALyraCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasAllMatchingGameplayTags(TagContainer);
	}
	
	return false;
}

bool ALyraCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent())
	{
		return LyraASC->HasAnyMatchingGameplayTags(TagContainer);
	}
	
	return false;
}

void ALyraCharacter::ToggleCrouch()
{
	const ULyraCharacterMovementComponent* LyraMoveCom = CastChecked<ULyraCharacterMovementComponent>(GetMovementComponent());
	
	if (IsCrouched() || LyraMoveCom->bWantsToCrouch)
	{
		UnCrouch();
	} 
	else if (LyraMoveCom->IsMovingOnGround())
	{
		Crouch();
	}
}

void ALyraCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	
	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (ULyraSignificanceManager* SignificanceManager = USignificanceManager::Get<ULyraSignificanceManager>(World))
		{
			
		}
	}
}

void ALyraCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UWorld* World = GetWorld();
	
	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (ULyraSignificanceManager* SignificanceManager = USignificanceManager::Get<ULyraSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void ALyraCharacter::Reset()
{
	DisableMovementAndCollision();
	
	// K2_ (Kismet 2，蓝图的前身) 开头的 C++ 方法，都意味着它是一个 专供蓝图实现（BlueprintImplementableEvent）的钩子函数。
	K2_OnReset();
	
	UninitAndDestroy();
}

void ALyraCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	DOREPLIFETIME(ThisClass, MyTeamID);
}

void ALyraCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	
	if (const UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		const double MaxAccel = MovementComponent->GetMaxAcceleration();
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		// 笛卡尔坐标系转换为极坐标系
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);
		
		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);
	}
}

void ALyraCharacter::NotifyControllerChanged()
{
	// Character 上记录的 TeamID
	const FGenericTeamId OldTeamID = GetGenericTeamId();
	
	Super::NotifyControllerChanged();
	
	if (HasAuthority() && (GetController() != nullptr))
	{
		if (ILyraTeamAgentInterface* ControllerWithTeam = Cast<ILyraTeamAgentInterface>(GetController()))
		{
			// 当前绑定的 PlayerController 的 TeamID
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamIndexChanged(this, OldTeamID, MyTeamID);
		}
	}
}

void ALyraCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamIndexChanged(this, OldTeamID, MyTeamID);
		} 
		else
		{
			UE_LOG(LogLyraTeams, Error, TEXT("You can't set team ID on a character (%s) expect on the authority"), *GetPathNameSafe(this));	
		}
	}
	else
	{
		UE_LOG(LogLyra, Error, TEXT("You can't set team ID on a character (%s); it's driven by  the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId ALyraCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

// 外部应仅Bind，不应BroadCast
FOnLyraTeamIndexChangedDelegate* ALyraCharacter::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void ALyraCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	// 是否处于回放模式
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}
	
	
}

bool ALyraCharacter::UpdateSharedReplication()
{
	return false;
}

void ALyraCharacter::OnAbilitySystemInitialized()
{
}

void ALyraCharacter::OnAbilitySystemUnInitialized()
{
}

void ALyraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ALyraCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void ALyraCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
}

void ALyraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ALyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ALyraCharacter::InitializeGameplayTags()
{
}

void ALyraCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	Super::FellOutOfWorld(dmgType);
}

void ALyraCharacter::OnDeathStarted(AActor* OwningActor)
{
}

void ALyraCharacter::OnDeathFinished(AActor* OwningActor)
{
}

void ALyraCharacter::DisableMovementAndCollision()
{
}

void ALyraCharacter::DestroyDueToDeath()
{
}

void ALyraCharacter::UninitAndDestroy()
{
}

void ALyraCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void ALyraCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 PreviousCustomMode)
{
}

void ALyraCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ALyraCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool ALyraCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation();
}

void ALyraCharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
}

void ALyraCharacter::OnRep_ReplicatedAcceleration()
{
}

void ALyraCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
}
