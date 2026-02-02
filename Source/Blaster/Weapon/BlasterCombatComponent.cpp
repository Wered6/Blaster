// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "BlasterWeaponTypes.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterCharacterOverlay.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Player/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


UBlasterCombatComponent::UBlasterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

void UBlasterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBlasterCombatComponent, EquippedWeapon)
	DOREPLIFETIME(UBlasterCombatComponent, bAiming)
	DOREPLIFETIME_CONDITION(UBlasterCombatComponent, CarriedAmmo, COND_OwnerOnly)
	DOREPLIFETIME(UBlasterCombatComponent, CombatState)
}

void UBlasterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	DefaultFOV = BlasterCharacter->GetCameraComponent()->FieldOfView;
	CurrentFOV = DefaultFOV;

	if (BlasterCharacter->IsLocallyControlled())
	{
		// if (!ensureMsgf(BlasterPlayerController.Get(), TEXT("Probably BlasterPlayerController is not set in Game Mode")))
		// {
		// 	return;
		// }
		// Not valid in LobbyGameMode because it is set to default controller
		if (!BlasterPlayerController.IsValid())
		{
			return;
		}
		BlasterHUD = BlasterPlayerController->GetHUD<ABlasterHUD>();
	}
	if (BlasterCharacter->HasAuthority())
	{
		InitializeCarriedAmmo();
	}
}

void UBlasterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (BlasterCharacter->IsLocallyControlled())
	{
		SetCrosshairsSpread(DeltaTime);

		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTargetLocation = HitResult.ImpactPoint;

		InterpFOV(DeltaTime);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBlasterCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case EBlasterCombatState::Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	case EBlasterCombatState::Reloading:
		HandleReload();
		break;
	default:
		break;
	}
}

void UBlasterCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EBlasterWeaponType::AssaultRifle, StartingARAmmo);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBlasterCombatComponent::OnRep_CarriedAmmo()
{
	BlasterPlayerController->SetHUDCarriedAmmo(CarriedAmmo);
}

void UBlasterCombatComponent::UpdateAmmoValues()
{
	const EBlasterWeaponType EquippedWeaponType{EquippedWeapon->GetWeaponType()};
	if (!CarriedAmmoMap.Contains(EquippedWeaponType))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: CarriedAmmoMap doesn't contain EquippedWeaponType"), TEXT(__FUNCTION__))
		return;
	}

	const int32 ReloadAmount{AmountToReload()};
	CarriedAmmoMap[EquippedWeaponType] -= ReloadAmount;
	CarriedAmmo = CarriedAmmoMap[EquippedWeaponType];
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UBlasterCombatComponent::SetHUDAmmo(const int32 InWeaponAmmo, const int32 InCarriedAmmo) const
{
	BlasterHUD->GetCharacterOverlay()->SetWeaponAmmoAmountText(InWeaponAmmo);
	BlasterHUD->GetCharacterOverlay()->SetCarriedAmmoAmountText(InCarriedAmmo);
}

void UBlasterCombatComponent::EquipWeapon(ABlasterWeaponBase* Weapon)
{
	if (!Weapon)
	{
		return;
	}

	if (!ensure(Weapon->GetEquipSound()))
	{
		return;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->Drop();
	}

	ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};

	EquippedWeapon = Weapon;
	EquippedWeapon->SetWeaponState(EBlasterWeaponState::Equipped);
	const USkeletalMeshSocket* HandSocket{BlasterCharacterRaw->GetMesh()->GetSocketByName(FName("RightHandSocket"))};
	if (!ensure(HandSocket))
	{
		return;
	}
	HandSocket->AttachActor(EquippedWeapon, BlasterCharacterRaw->GetMesh());

	const EBlasterWeaponType EquippedWeaponType{EquippedWeapon->GetWeaponType()};
	if (!CarriedAmmoMap.Contains(EquippedWeaponType))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: CarriedAmmoMap doesn't contain EquippedWeaponType"), TEXT(__FUNCTION__))
		return;
	}
	CarriedAmmo = CarriedAmmoMap[EquippedWeaponType];

	EquippedWeapon->SetOwner(BlasterCharacterRaw);

	UGameplayStatics::PlaySoundAtLocation(
		this,
		EquippedWeapon->GetEquipSound(),
		BlasterCharacter->GetActorLocation()
	);

	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}

	BlasterCharacterRaw->GetCharacterMovement()->bOrientRotationToMovement = false;
	BlasterCharacterRaw->bUseControllerRotationYaw = true;
	if (BlasterCharacter->IsLocallyControlled())
	{
		if (!ensureMsgf(BlasterHUD.Get(), TEXT("Machine: %d"), UE::GetPlayInEditorID()))
		{
			return;
		}
		BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
		SetHUDAmmo(EquippedWeapon->GetAmmo(), CarriedAmmo);
	}
}

void UBlasterCombatComponent::DropWeapon()
{
	// Called only on server
	if (EquippedWeapon)
	{
		EquippedWeapon->Drop();
		EquippedWeapon = nullptr;
		if (BlasterCharacter->IsLocallyControlled())
		{
			BlasterHUD->ResetCrosshairsPackage();
			SetHUDAmmo(0, 0);
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBlasterCombatComponent::OnRep_EquippedWeapon()
{
	ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};

	if (!EquippedWeapon)
	{
		if (BlasterCharacterRaw->IsLocallyControlled())
		{
			BlasterHUD->ResetCrosshairsPackage();
			SetHUDAmmo(0, 0);
		}
		return;
	}

	EquippedWeapon->SetWeaponState(EBlasterWeaponState::Equipped);
	const USkeletalMeshSocket* HandSocket{BlasterCharacterRaw->GetMesh()->GetSocketByName(FName("RightHandSocket"))};
	if (!ensure(HandSocket))
	{
		return;
	}
	HandSocket->AttachActor(EquippedWeapon, BlasterCharacterRaw->GetMesh());

	BlasterCharacterRaw->GetCharacterMovement()->bOrientRotationToMovement = false;
	BlasterCharacterRaw->bUseControllerRotationYaw = true;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		EquippedWeapon->GetEquipSound(),

		BlasterCharacter->GetActorLocation()
	);

	if (BlasterCharacterRaw->IsLocallyControlled())
	{
		BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
		SetHUDAmmo(EquippedWeapon->GetAmmo(), CarriedAmmo);
	}
}

bool UBlasterCombatComponent::CanFire() const
{
	if (!EquippedWeapon)
	{
		return false;
	}

	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == EBlasterCombatState::Unoccupied;
}

void UBlasterCombatComponent::FireStart()
{
	if (!EquippedWeapon)
	{
		return;
	}

	bFireButtonPressed = true;
	Fire();
}

void UBlasterCombatComponent::FireStop()
{
	bFireButtonPressed = false;
}

void UBlasterCombatComponent::Fire()
{
	if (CanFire())
	{
		bCanFire = false;
		Server_Fire(HitTargetLocation);
		CrosshairShootingFactor = 0.75f;
		StartFireTimer();
	}
}

void UBlasterCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	Multicast_Fire(TraceHitTargetLocation);
}

void UBlasterCombatComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	if (CombatState == EBlasterCombatState::Unoccupied)
	{
		BlasterCharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTargetLocation);
	}
}

void UBlasterCombatComponent::StartFireTimer()
{
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UBlasterCombatComponent::OnFireTimerCompleted, EquippedWeapon->GetFireDelay());
}

void UBlasterCombatComponent::OnFireTimerCompleted()
{
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->IsAutomatic())
	{
		Fire();
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UBlasterCombatComponent::SetAiming(const bool bInAiming)
{
	bAiming = bInAiming;
	BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = bInAiming ? AimWalkSpeed : BaseWalkSpeed;

	Server_SetAiming(bInAiming);
}

void UBlasterCombatComponent::Server_SetAiming_Implementation(const bool bInAiming)
{
	bAiming = bInAiming;
	BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = bInAiming ? AimWalkSpeed : BaseWalkSpeed;
}

void UBlasterCombatComponent::InterpFOV(const float DeltaTime)
{
	if (!EquippedWeapon)
	{
		return;
	}

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	BlasterCharacter->GetCameraComponent()->SetFieldOfView(CurrentFOV);
}

void UBlasterCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	const FVector2D CrosshairLocation(ViewportSize / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld{
		UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldPosition,
			CrosshairWorldDirection
		)
	};

	if (bScreenToWorld)
	{
		FVector Start{CrosshairWorldPosition};
		const float DistanceToCharacter = (BlasterCharacter->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

		const FVector End{Start + CrosshairWorldDirection * TRACE_LENGTH};

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);

		TargetActor = Cast<IBlasterInteractWithCrosshairsInterface>(TraceHitResult.GetActor());

		if (BlasterHUD.Get())
		{
			BlasterHUD->SetCrosshairsColor(TargetActor ? FLinearColor::Red : FLinearColor::White);
		}
	}
}

void UBlasterCombatComponent::SetCrosshairsSpread(const float DeltaTime)
{
	if (!EquippedWeapon)
	{
		return;
	}

	// Calculate crosshair spread
	const FVector2D WalkSpeedRange(0.f, BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity{BlasterCharacter->GetVelocity()};
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (BlasterCharacter->GetCharacterMovement()->IsFalling())
	{
		CrosshairAirborneFactor = FMath::FInterpTo(CrosshairAirborneFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairAirborneFactor = FMath::FInterpTo(CrosshairAirborneFactor, 0.f, DeltaTime, 30.f);
	}

	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, bAiming ? 0.4f : 0.f, DeltaTime, 30.f);
	CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);
	CrosshairAimAtPlayerFactor = FMath::FInterpTo(CrosshairAimAtPlayerFactor, TargetActor ? 0.3f : 0.f, DeltaTime, 30.f);

	const float CrosshairSpread{
		0.6f +
		CrosshairVelocityFactor +
		CrosshairAirborneFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor -
		CrosshairAimAtPlayerFactor
	};
	if (!ensureMsgf(BlasterHUD.Get(), TEXT("Machine: %d"), UE::GetPlayInEditorID()))
	{
		return;
	}
	BlasterHUD->SetCrosshairsSpread(CrosshairSpread);
}

void UBlasterCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != EBlasterCombatState::Reloading)
	{
		Server_Reload();
	}
}

void UBlasterCombatComponent::FinishReloading()
{
	if (BlasterCharacter->HasAuthority())
	{
		UpdateAmmoValues();
		CombatState = EBlasterCombatState::Unoccupied;
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UBlasterCombatComponent::Server_Reload_Implementation()
{
	CombatState = EBlasterCombatState::Reloading;
	HandleReload();
}

void UBlasterCombatComponent::HandleReload() const
{
	BlasterCharacter->PlayReloadMontage();
}

int32 UBlasterCombatComponent::AmountToReload()
{
	const int32 RoomInMag{EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo()};
	const int32 CarriedAmmoAmount{CarriedAmmoMap[EquippedWeapon->GetWeaponType()]};
	const int32 Least{FMath::Min(RoomInMag, CarriedAmmoAmount)};

	return Least;
}
