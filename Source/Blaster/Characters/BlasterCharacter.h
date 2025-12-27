// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

#pragma region Camera

private:
	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, Category="Blaster|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion
};
