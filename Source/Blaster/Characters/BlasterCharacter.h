// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

enum class ETurningInPlace : uint8;
class UBlasterCombatComponent;
class ABlasterWeaponBase;
class UWidgetComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left UMETA(DisplayName="Turning Left"),
	Right UMETA(DisplayName="Turning Right"),
	NotTurning UMETA(DisplayName="Not Turning"),

	MAX UMETA(DisplayName="DefaultMAX")
};

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

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
	void Crouch();
	void AimStart();
	void AimStop();

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
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> AimAction;

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

#pragma region Weapon

public:
	void SetOverlappingWeapon(ABlasterWeaponBase* Weapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;

	ABlasterWeaponBase* GetEquippedWeapon() const;

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(ABlasterWeaponBase* LastWeapon);

	UFUNCTION(Server, Reliable)
	void Server_Equip();

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	TObjectPtr<ABlasterWeaponBase> OverlappingWeapon;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Combat")
	TObjectPtr<UBlasterCombatComponent> CombatComponent;

#pragma endregion

#pragma region Movement

public:
	FORCEINLINE float GetYawAimOffest() const
	{
		return YawAimOffest;
	}

	FORCEINLINE float GetPitchAimOffset() const
	{
		return PitchAimOffset;
	}

	FORCEINLINE ETurningInPlace GetTurningInPlace() const
	{
		return TurningInPlace;
	}

protected:
	void AimOffset(const float DeltaTime);

private:
	void TurnInPlace(float DeltaTime);
	
	float YawAimOffest;
	float PitchAimOffset;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

#pragma endregion
};
