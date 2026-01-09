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
		BlasterPlayerController = Cast<ABlasterPlayerController>(BlasterCharacter->GetController());
		if (!ensureMsgf(BlasterPlayerController.Get(), TEXT("Probably BlasterPlayerController is not set in Game Mode")))
		{
			return;
		}

		BlasterHUD = Cast<ABlasterHUD>(BlasterPlayerController->GetHUD());
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
		BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UBlasterCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};

		BlasterCharacterRaw->GetCharacterMovement()->bOrientRotationToMovement = false;
		BlasterCharacterRaw->bUseControllerRotationYaw = true;
		if (BlasterCharacterRaw->IsLocallyControlled())
		{
			BlasterHUD->SetCrosshairsPackage(EquippedWeapon->GetCrosshairsPackage());
		}
	}
}

void UBlasterCombatComponent::FireStart()
{
	bFireButtonPressed = true;

	if (!EquippedWeapon)
	{
		return;;
	}

	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);

	Server_Fire(HitResult.ImpactPoint);

	CrosshairShootingFactor = 0.75f;
}

void UBlasterCombatComponent::FireStop()
{
	bFireButtonPressed = false;
}

void UBlasterCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult) const
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
		const FVector Start{CrosshairWorldPosition};
		const FVector End{Start + CrosshairWorldDirection * TRACE_LENGTH};

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Visibility
		);
	}
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

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

	const float CrosshairSpread{
		0.5f +
		CrosshairVelocityFactor +
		CrosshairAirborneFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor
	};
	BlasterHUD->SetCrosshairSpread(CrosshairSpread);
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
