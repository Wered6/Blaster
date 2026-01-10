// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterProjectile.h"
#include "Blaster/Blaster.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ABlasterProjectile::ABlasterProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>("CollisionBoxComponent");
	SetRootComponent(CollisionBoxComponent);
	CollisionBoxComponent->SetBoxExtent(FVector(5.f, 2.5f, 2.5f));
	CollisionBoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBoxComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBoxComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBoxComponent->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 15000.f;
	ProjectileMovementComponent->MaxSpeed = 15000.f;
}

void ABlasterProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(TracerParticleSystem))
	{
		return;
	}

	TracerParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
		TracerParticleSystem,
		CollisionBoxComponent,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition
	);

	if (HasAuthority())
	{
		CollisionBoxComponent->OnComponentHit.AddDynamic(this, &ABlasterProjectile::OnHit);
	}
}

void ABlasterProjectile::Destroyed()
{
	Super::Destroyed();

	if (!ensure(ImpactParticleSystem && ImpactSound))
	{
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticleSystem, GetActorTransform());
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}

void ABlasterProjectile::OnHit(UPrimitiveComponent* HitComponent,
                               AActor* OtherActor,
                               UPrimitiveComponent* OtherComp,
                               FVector NormalImpulse,
                               const FHitResult& Hit)
{
	Destroy();
}
