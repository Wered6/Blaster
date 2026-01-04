// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterProjectile.h"
#include "Components/BoxComponent.h"


ABlasterProjectile::ABlasterProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>("CollisionBoxComponent");
	SetRootComponent(CollisionBoxComponent);
	CollisionBoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FCollisionResponseContainer ResponseContainer;
	ResponseContainer.SetAllChannels(ECR_Ignore);
	ResponseContainer.SetResponse(ECC_Visibility, ECR_Block);
	ResponseContainer.SetResponse(ECC_WorldStatic, ECR_Block);
	CollisionBoxComponent->SetCollisionResponseToChannels(ResponseContainer);
}
