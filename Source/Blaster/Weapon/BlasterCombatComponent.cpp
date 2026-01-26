// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Player/BlasterPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


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
}

void UBlasterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	BlasterCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	DefaultFOV = BlasterCharacter->GetCameraComponent()->FieldOfView;
	CurrentFOV = DefaultFOV;

	if (BlasterCharacter->IsLocallyControlled())
	{
		if (!ensureMsgf(BlasterPlayerController.Get(), TEXT("Probably BlasterPlayerController is not set in Game Mode")))
		{
			return;
		}
		BlasterHUD = BlasterPlayerController->GetHUD<ABlasterHUD>();
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

void UBlasterCombatComponent::EquipWeapon(ABlasterWeaponBase* Weapon)
{
	if (!Weapon)
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

	EquippedWeapon->SetOwner(BlasterCharacterRaw);

	BlasterCharacterRaw->GetCharacterMovement()->bOrientRotationToMovement = false;
	BlasterCharacterRaw->bUseControllerRotationYaw = true;
	if (BlasterCharacter->IsLocallyControlled())
	{
		if (!ensureMsgf(BlasterHUD.Get(), TEXT("Machine: %d"), UE::GetPlayInEditorID()))
		{
			return;
		}
		BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
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

	if (BlasterCharacterRaw->IsLocallyControlled())
	{
		BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
	}
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
}

bool UBlasterCombatComponent::CanFire() const
{
	if (!EquippedWeapon)
	{
		return false;
	}

	return !EquippedWeapon->IsEmpty() || !bCanFire;
}

void UBlasterCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	Multicast_Fire(TraceHitTargetLocation);
}

void UBlasterCombatComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	BlasterCharacter->PlayFireMontage(bAiming);
	EquippedWeapon->Fire(TraceHitTargetLocation);
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
