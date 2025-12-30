// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterWeapon.h"
#include "Blaster/Characters/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


ABlasterWeapon::ABlasterWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMeshComponent");
	SetRootComponent(WeaponMeshComponent);
	WeaponMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphereComponent = CreateDefaultSubobject<USphereComponent>("AreaSphereComponent");
	AreaSphereComponent->SetupAttachment(RootComponent);
	AreaSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("PickUpWidgetComponent");
	PickUpWidgetComponent->SetupAttachment(RootComponent);
}

void ABlasterWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlasterWeapon::OnAreaSphereOverlap);
	}

	if (PickUpWidgetComponent)
	{
		PickUpWidgetComponent->SetVisibility(false);
	}
}

void ABlasterWeapon::OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent,
                                         AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex,
                                         bool bFromSweep,
                                         const FHitResult& SweepResult)
{
	const ABlasterCharacter* BlasterCharacter{Cast<ABlasterCharacter>(OtherActor)};
	if (BlasterCharacter && PickUpWidgetComponent)
	{
		PickUpWidgetComponent->SetVisibility(true);
	}
}
