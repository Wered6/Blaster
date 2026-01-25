// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterProjectileWeapon.h"
#include "BlasterProjectile.h"
#include "Engine/SkeletalMeshSocket.h"


ABlasterProjectileWeapon::ABlasterProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABlasterProjectileWeapon::Fire(const FVector& HitTargetLocation)
{
	Super::Fire(HitTargetLocation);

	if (!HasAuthority())
	{
		return;
	}

	if (!ensure(ProjectileClass))
	{
		return;
	}

	const USkeletalMeshSocket* MuzzleFlashSocket{GetWeaponMeshComponent()->GetSocketByName(FName("MuzzleFlash"))};
	if (!ensureMsgf(MuzzleFlashSocket, TEXT("Wrong socket name for muzzle flash")))
	{
		return;
	}

	const FTransform SocketTransform{MuzzleFlashSocket->GetSocketTransform(GetWeaponMeshComponent())};
	// from muzzle flash socket to hit location from TraceUnderCrosshairs
	const FVector ToTargetLocation{HitTargetLocation - SocketTransform.GetLocation()};
	const FRotator TargetRotation{ToTargetLocation.Rotation()};

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = GetOwner<APawn>();
	GetWorld()->SpawnActor<ABlasterProjectile>(
		ProjectileClass,
		SocketTransform.GetLocation(),
		TargetRotation,
		SpawnParameters
	);
}
