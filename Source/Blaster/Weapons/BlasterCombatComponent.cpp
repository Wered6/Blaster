// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "Blaster/Characters/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"


UBlasterCombatComponent::UBlasterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterCombatComponent::EquipWeapon(ABlasterWeaponBase* Weapon)
{
	if (!ensure(BlasterCharacter && Weapon))
	{
		return;
	}

	EquippedWeapon = Weapon;
	EquippedWeapon->SetWeaponState(EBlasterWeaponState::Equipped);
	const USkeletalMeshSocket* HandSocket{BlasterCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"))};
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());
	}

	EquippedWeapon->SetOwner(BlasterCharacter);
	EquippedWeapon->ShowPickUpWidget(false);
}
