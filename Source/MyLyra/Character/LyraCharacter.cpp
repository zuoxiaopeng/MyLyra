#include "LyraCharacter.h"

#include "LyraCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

ALyraPlayerController* ALyraCharacter::GetLyraPlayerController() const
{
	return nullptr;
}

ALyraPlayerState* ALyraCharacter::GetLyraPlayerState() const
{
	return nullptr;
}

UAbilitySystemComponent* ALyraCharacter::GetLyraAbilitySystemComponent() const
{
	return nullptr;
}

UAbilitySystemComponent* ALyraCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ALyraCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
}

bool ALyraCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return IGameplayTagAssetInterface::HasMatchingGameplayTag(TagToCheck);
}

bool ALyraCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAllMatchingGameplayTags(TagContainer);
}

bool ALyraCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return IGameplayTagAssetInterface::HasAnyMatchingGameplayTags(TagContainer);
}

void ALyraCharacter::ToggleCrouch()
{
}

void ALyraCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ALyraCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ALyraCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ALyraCharacter::Reset()
{
	Super::Reset();
}

void ALyraCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ALyraCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void ALyraCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ALyraCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	ILyraTeamAgentInterface::SetGenericTeamId(TeamID);
}

FGenericTeamId ALyraCharacter::GetGenericTeamId() const
{
	return ILyraTeamAgentInterface::GetGenericTeamId();
}

FOnLyraTeamIndexChangedDelegate* ALyraCharacter::GetOnTeamIndexChangedDelegate()
{
	return ILyraTeamAgentInterface::GetOnTeamIndexChangedDelegate();
}

void ALyraCharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
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
