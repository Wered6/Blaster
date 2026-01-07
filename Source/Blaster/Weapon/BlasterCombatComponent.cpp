// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Player/BlasterPlayerController.h"
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
	BlasterPlayerController = Cast<ABlasterPlayerController>(BlasterCharacter->GetController());
	if (!ensureMsgf(BlasterPlayerController.Get(), TEXT("Probably BlasterPlayerController is not set in Game Mode")))
	{
		return;
	}

	BlasterHUD = Cast<ABlasterHUD>(BlasterPlayerController->GetHUD());
}

void UBlasterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);
}

void UBlasterCombatComponent::EquipWeapon(ABlasterWeaponBase* Weapon)
{
	ABlasterCharacter* BlasterCharacterRaw{BlasterCharacter.Get()};

	if (!Weapon)
	{
		return;
	}

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
	}
}

void UBlasterCombatComponent::FireStart()
{
	bFireButtonPressed = true;

	FHitResult HitResult;
	TraceUnderCrosshairs(HitResult);

	Server_Fire(HitResult.ImpactPoint);
}

void UBlasterCombatComponent::FireStop()
{
	bFireButtonPressed = false;
}

void UBlasterCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult) const
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

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

void UBlasterCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	FBlasterCrosshairsPackage CrosshairPackage{EquippedWeapon ? EquippedWeapon->GetCrosshairsPackage() : FBlasterCrosshairsPackage()};
}

void UBlasterCombatComponent::Server_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	Multicast_Fire(TraceHitTargetLocation);
}

void UBlasterCombatComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& TraceHitTargetLocation)
{
	if (!EquippedWeapon)
	{
		return;
	}

	BlasterCharacter->PlayFireMontage(bAiming);
	EquippedWeapon->Fire(TraceHitTargetLocation);
}
