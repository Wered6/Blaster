// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterWeaponBase.h"
#include "BlasterCasing.h"
#include "BlasterCombatComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Player/BlasterPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"


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
	AreaSphereComponent->SetRelativeLocation(FVector(0.f, 25.f, 12.f));
	AreaSphereComponent->SetSphereRadius(76.f);

	PickUpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("PickUpWidgetComponent");
	PickUpWidgetComponent->SetupAttachment(RootComponent);
	PickUpWidgetComponent->SetRelativeLocation(FVector(0.f, 30.f, 40.f));
	PickUpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	PickUpWidgetComponent->SetDrawAtDesiredSize(true);
}

void ABlasterWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterWeaponBase, WeaponState)
	DOREPLIFETIME(ABlasterWeaponBase, Ammo)
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

	PickUpWidgetComponent->SetVisibility(false);
}

void ABlasterWeaponBase::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	BlasterOwnerCharacter = Cast<ABlasterCharacter>(NewOwner);

	if (BlasterOwnerCharacter.IsValid())
	{
		BlasterOwnerPlayerController = BlasterOwnerCharacter->GetController<ABlasterPlayerController>();
	}
	else
	{
		BlasterOwnerPlayerController = nullptr;
	}
}

void ABlasterWeaponBase::ShowPickUpWidget(const bool bShowWidget) const
{
	if (PickUpWidgetComponent)
	{
		PickUpWidgetComponent->SetVisibility(bShowWidget);
	}
}

void ABlasterWeaponBase::SetWeaponState(const EBlasterWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EBlasterWeaponState::Equipped:
		ShowPickUpWidget(false);
		AreaSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMeshComponent->SetSimulatePhysics(false);
		WeaponMeshComponent->SetEnableGravity(false);
		WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EBlasterWeaponState::Dropped:
		if (HasAuthority())
		{
			AreaSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMeshComponent->SetSimulatePhysics(true);
		WeaponMeshComponent->SetEnableGravity(true);
		WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
}

void ABlasterWeaponBase::Drop()
{
	SetOwner(nullptr);
	SetWeaponState(EBlasterWeaponState::Dropped);
	WeaponMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterWeaponBase::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EBlasterWeaponState::Equipped:
		ShowPickUpWidget(false);
		WeaponMeshComponent->SetSimulatePhysics(false);
		WeaponMeshComponent->SetEnableGravity(false);
		WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EBlasterWeaponState::Dropped:
		WeaponMeshComponent->SetSimulatePhysics(true);
		WeaponMeshComponent->SetEnableGravity(true);
		WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	default:
		break;
	}
}

void ABlasterWeaponBase::AddAmmo(const int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);

	SetHUDWeaponAmmo(Ammo);
	SetHUDCarriedAmmo(BlasterOwnerCharacter->GetCombatComponent()->GetCarriedAmmo());
}

void ABlasterWeaponBase::SetHUDWeaponAmmo(const int32 AmmoAmount) const
{
	const ABlasterPlayerController* BlasterOwnerPlayerControllerRaw{BlasterOwnerPlayerController.Get()};
	if (BlasterOwnerPlayerControllerRaw && BlasterOwnerPlayerControllerRaw->IsLocalController())
	{
		BlasterOwnerPlayerControllerRaw->SetHUDWeaponAmmo(AmmoAmount);
	}
}

void ABlasterWeaponBase::SetHUDCarriedAmmo(const int32 AmmoAmount) const
{
	const ABlasterPlayerController* BlasterOwnerPlayerControllerRaw{BlasterOwnerPlayerController.Get()};
	if (BlasterOwnerPlayerControllerRaw && BlasterOwnerPlayerControllerRaw->IsLocalController())
	{
		BlasterOwnerPlayerControllerRaw->SetHUDCarriedAmmo(AmmoAmount);
	}
}

void ABlasterWeaponBase::SpendRound()
{
	if (Ammo > 0)
	{
		Ammo--;
	}

	SetHUDWeaponAmmo(Ammo);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterWeaponBase::OnRep_Ammo()
{
	SetHUDWeaponAmmo(Ammo);
}

void ABlasterWeaponBase::Fire(const FVector& HitTargetLocation)
{
	if (!ensure(FireAnimation && CasingClass))
	{
		return;
	}

	WeaponMeshComponent->PlayAnimation(FireAnimation, false);

	const USkeletalMeshSocket* AmmoEjectSocket{WeaponMeshComponent->GetSocketByName(FName("AmmoEject"))};
	if (!ensureMsgf(AmmoEjectSocket, TEXT("Wrong socket name for ammo eject")))
	{
		return;
	}

	const FTransform SocketTransform{AmmoEjectSocket->GetSocketTransform(WeaponMeshComponent)};
	GetWorld()->SpawnActor<ABlasterCasing>(CasingClass, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator());

	if (HasAuthority())
	{
		SpendRound();
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
