// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UBlasterCombatComponent;
class ABlasterWeaponBase;
class UWidgetComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

private:
	void ShowPlayerName() const;

#pragma region Input

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip();

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> EquipAction;

#pragma endregion

#pragma region Camera

private:
	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region HUD

protected:
	UFUNCTION(BlueprintPure)
	UWidgetComponent* GetOverheadWidgetComponent() const
	{
		return OverheadWidgetComponent;
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintGetter=GetOverheadWidgetComponent, Category="Blaster|HUD")
	TObjectPtr<UWidgetComponent> OverheadWidgetComponent;

#pragma endregion

public:
	void SetOverlappingWeapon(ABlasterWeaponBase* Weapon);
	bool IsWeaponEquipped() const;

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABlasterWeaponBase* LastWeapon);

	UFUNCTION(Server, Reliable)
	void Server_Equip();

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	TObjectPtr<ABlasterWeaponBase> OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Combat")
	TObjectPtr<UBlasterCombatComponent> CombatComponent;
};
