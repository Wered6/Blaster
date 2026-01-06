// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCasing.h"


ABlasterCasing::ABlasterCasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("CashingMeshComponent");
	SetRootComponent(CasingMeshComponent);
}
