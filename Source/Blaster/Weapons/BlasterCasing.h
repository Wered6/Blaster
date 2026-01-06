// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterCasing.generated.h"

UCLASS()
class BLASTER_API ABlasterCasing : public AActor
{
	GENERATED_BODY()

public:
	ABlasterCasing();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CasingMeshComponent;
};
