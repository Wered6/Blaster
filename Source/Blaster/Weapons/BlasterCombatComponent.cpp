// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "Blaster/Characters/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


UBlasterCombatComponent::UBlasterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBlasterCombatComponent, EquippedWeapon)
	DOREPLIFETIME(UBlasterCombatComponent, bAiming)
}

void UBlasterCombatComponent::EquipWeapon(ABlasterWeaponBase* Weapon)
{
	if (!ensure(BlasterCharacter))
	{
		return;
	}

	if (!Weapon)
	{
		return;
	}

	EquippedWeapon = Weapon;
	EquippedWeapon->SetWeaponState(EBlasterWeaponState::Equipped);
	const USkeletalMeshSocket* HandSocket{BlasterCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"))};
	if (!ensure(HandSocket))
	{
		return;
	}
	HandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());

	EquippedWeapon->SetOwner(BlasterCharacter);

	BlasterCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	BlasterCharacter->bUseControllerRotationYaw = true;
}

void UBlasterCombatComponent::SetAiming(const bool bInAiming)
{
	bAiming = bInAiming;

	Server_SetAiming(bInAiming);
}

void UBlasterCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon)
	{
		BlasterCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		BlasterCharacter->bUseControllerRotationYaw = true;
	}
}

void UBlasterCombatComponent::Server_SetAiming_Implementation(const bool bInAiming)
{
	bAiming = bInAiming;
}
