// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterWeaponBase.generated.h"

class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EBlasterWeaponState : uint8
{
	Initial UMETA(DisplayName="Initial State"),
	Equipped UMETA(DisplayName="Equipped"),
	Dropped UMETA(DisplayName="Dropped"),

	MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class BLASTER_API ABlasterWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	ABlasterWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ShowPickUpWidget(const bool bShowWidget) const;

	void SetWeaponState(const EBlasterWeaponState State);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAreaSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                                      AActor* OtherActor,
	                                      UPrimitiveComponent* OtherComp,
	                                      int32 OtherBodyIndex,
	                                      bool bFromSweep,
	                                      const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnAreaSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	                                    AActor* OtherActor,
	                                    UPrimitiveComponent* OtherComp,
	                                    int32 OtherBodyIndex);

private:
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category="Blaster|Weapon Properties")
	EBlasterWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphereComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Weapon Properties")
	TObjectPtr<UWidgetComponent> PickUpWidgetComponent;
};
