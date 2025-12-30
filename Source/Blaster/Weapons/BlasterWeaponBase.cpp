// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterWeaponBase.h"
#include "Blaster/Characters/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


ABlasterWeaponBase::ABlasterWeaponBase()
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

void ABlasterWeaponBase::ShowPickUpWidget(const bool bShowWidget) const
{
	if (PickUpWidgetComponent)
	{
		PickUpWidgetComponent->SetVisibility(bShowWidget);
	}
}

void ABlasterWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABlasterWeaponBase::OnAreaSphereBeginOverlap);
		AreaSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABlasterWeaponBase::OnAreaSphereEndOverlap);
	}

	if (PickUpWidgetComponent)
	{
		PickUpWidgetComponent->SetVisibility(false);
	}
}

void ABlasterWeaponBase::OnAreaSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                              AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp,
                                              int32 OtherBodyIndex,
                                              bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter{Cast<ABlasterCharacter>(OtherActor)};
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void ABlasterWeaponBase::OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
                                            AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter{Cast<ABlasterCharacter>(OtherActor)};
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}
