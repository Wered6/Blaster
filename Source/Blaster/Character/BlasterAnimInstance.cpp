// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "Blaster/Weapon/BlasterWeaponBase.h"
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

	bAirborne = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bCrouched = BlasterCharacter->bIsCrouched;
	bAiming = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

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

	const USkeletalMeshComponent* CharacterMeshComponent{BlasterCharacter->GetMesh()};
	if (bWeaponEquipped)
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMeshComponent()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);

		FVector OutPosition;
		FRotator OutRotation;
		CharacterMeshComponent->TransformToBoneSpace(
			FName("hand_r"),
			LeftHandTransform.GetLocation(),
			FRotator::ZeroRotator,
			OutPosition,
			OutRotation
		);

		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;

			const FTransform RightHandTransform{CharacterMeshComponent->GetSocketTransform(FName("hand_r"), RTS_World)};
			const FVector RightHandLocation{RightHandTransform.GetLocation()};
			const FVector TargetLocation{RightHandLocation + (RightHandLocation - BlasterCharacter->GetHitTargetLocation())};
			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandLocation, TargetLocation);
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		}
	}
}
