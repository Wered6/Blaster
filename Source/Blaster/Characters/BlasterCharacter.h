// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

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
	
	virtual void BeginPlay() override;

#pragma region Movement

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

#pragma endregion
	
#pragma region Input

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category="Blaster|Input")
	TObjectPtr<UInputAction> JumpAction;
	
#pragma endregion
	
#pragma region Camera

private:
	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion
};
