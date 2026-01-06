// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCasing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ABlasterCasing::ABlasterCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CashingMeshComponent");
	SetRootComponent(CasingMeshComponent);
	CasingMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CasingMeshComponent->SetSimulatePhysics(true);
	CasingMeshComponent->SetEnableGravity(true);
	CasingMeshComponent->SetNotifyRigidBodyCollision(true);

	ShellEjectionImpulse = 10.f;
}

void ABlasterCasing::OnHit(UPrimitiveComponent* HitComponent,
                           AActor* OtherActor,
                           UPrimitiveComponent* OtherComp,
                           FVector NormalImpulse,
                           const FHitResult& Hit)
{
	if (!ensure(ShellSound))
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(
		DestroyTimerHandle,
		[this]
		{
			Destroy();
		},
		2.f,
		false
	);

	CasingMeshComponent->SetNotifyRigidBodyCollision(false);
}

void ABlasterCasing::BeginPlay()
{
	Super::BeginPlay();

	const FVector RandomShellDirection{FMath::VRandCone(GetActorForwardVector(), FMath::DegreesToRadians(15.f))};
	CasingMeshComponent->AddImpulse(RandomShellDirection * ShellEjectionImpulse);
	CasingMeshComponent->OnComponentHit.AddDynamic(this, &ABlasterCasing::OnHit);
}
