// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "Blaster/Weapons/BlasterWeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!BlasterCharacter)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (!BlasterCharacter)
	{
		return;
	}

	FVector Velocity{BlasterCharacter->GetVelocity()};
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	const ABlasterWeaponBase* EquippedWeaponRaw = EquippedWeapon.Get();

	bAirborne = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();

	// Offset Yaw for Strafing
	const FRotator AimRotation{BlasterCharacter->GetBaseAimRotation()};
	const FRotator MovementRotation{UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity())};
	const FRotator DeltaRot{UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation)};
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	LastFrameRotation = CurrentRotation;
	CurrentRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta{UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, LastFrameRotation)};
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp{FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f)};
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	YawAimOffset = BlasterCharacter->GetYawAimOffest();
	PitchAimOffset = BlasterCharacter->GetPitchAimOffset();

	if (bWeaponEquipped && EquippedWeaponRaw && EquippedWeaponRaw->GetWeaponMeshComponent() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeaponRaw->GetWeaponMeshComponent()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
