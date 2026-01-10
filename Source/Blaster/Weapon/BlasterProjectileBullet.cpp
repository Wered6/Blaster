// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterProjectileBullet.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABlasterProjectileBullet::ABlasterProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABlasterProjectileBullet::OnHit(UPrimitiveComponent* HitComponent,
                                     AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp,
                                     FVector NormalImpulse,
                                     const FHitResult& Hit)
{
	ACharacter* OwnerCharacter{Cast<ACharacter>(GetOwner())};
	if (!ensure(OwnerCharacter))
	{
		return;
	}

	AController* OwnerController{OwnerCharacter->Controller};

	UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());

	// call it last because it destroys the projectile
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
