// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCombatComponent.h"
#include "BlasterWeaponBase.h"
#include "Blaster/Characters/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"


UBlasterCombatComponent::UBlasterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBlasterCombatComponent, EquippedWeapon)
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
	if (!ensure(HandSocket))
	{
		return;
	}
	HandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());

	EquippedWeapon->SetOwner(BlasterCharacter);
}
