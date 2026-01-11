// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blaster/Weapon/BlasterInteractWithCrosshairsInterface.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class ABlasterPlayerController;
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
class BLASTER_API ABlasterCharacter : public ACharacter, public IBlasterInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnRep_PlayerState() override;

private:
	void ShowPlayerName() const;

	TWeakObjectPtr<ABlasterPlayerController> BlasterPlayerController;

#pragma region Elimination

public:
	void Eliminate();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Eliminate();
	void PlayEliminationMontage() const;

	FORCEINLINE bool IsEliminated() const
	{
		return bEliminated;
	}

	void EliminationTimerCompleted();
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Elimination")
	TObjectPtr<UAnimMontage> EliminationMontage;

	bool bEliminated{false};
	
	FTimerHandle EliminationTimer;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Elimination")
	float EliminationDelay{3.f};

#pragma endregion

#pragma region Health

private:
	UFUNCTION()
	void OnDamageTaken(AActor* DamagedActor,
	                   float Damage,
	                   const UDamageType* DamageType,
	                   AController* InstigatedBy,
	                   AActor* DamageCauser);

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Stats")
	float MaxHealth{100.f};

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="Blaster|Stats")
	float Health{100.f};

#pragma endregion

#pragma region Camera

private:
	void HideCameraIfCameraClose() const;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Camera")
	float CameraThreshold{200.f};

#pragma endregion

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
	void FireStart();
	void FireStop();
	virtual void Jump() override;

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
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> FireAction;

#pragma endregion

#pragma region Combat

public:
	void SetOverlappingWeapon(ABlasterWeaponBase* Weapon);
	bool IsWeaponEquipped() const;
	bool IsAiming() const;

	ABlasterWeaponBase* GetEquippedWeapon() const;

	void PlayFireMontage(const bool bAiming) const;

	FVector GetHitTargetLocation() const;

private:
	void PlayHitReactMontage() const;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ABlasterWeaponBase* LastWeapon);

	UFUNCTION(Server, Reliable)
	void Server_Equip();

	UPROPERTY(ReplicatedUsing=OnRep_OverlappingWeapon)
	TObjectPtr<ABlasterWeaponBase> OverlappingWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Blaster|Combat")
	TObjectPtr<UAnimMontage> FireWeaponMontage;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

#pragma endregion

#pragma region Movement

public:
	virtual void OnRep_ReplicatedMovement() override;

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

	FORCEINLINE bool ShouldRotateRootBone() const
	{
		return bRotateRootBone;
	}

protected:
	void AimOffset(const float DeltaTime);
	void SimProxiesTurn();

private:
	void TurnInPlace(float DeltaTime);

	void CalculatePitchAimOffset();
	float CalculateSpeed() const;

	float YawAimOffest;
	float InterpYawAimOffset;
	float PitchAimOffset;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	bool bRotateRootBone;
	float TurnThreshold{0.5f};
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

#pragma endregion

#pragma region Components

public:
	FORCEINLINE UCameraComponent* GetCameraComponent() const
	{
		return CameraComponent;
	}

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> OverheadWidgetComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBlasterCombatComponent> CombatComponent;

#pragma endregion
};
