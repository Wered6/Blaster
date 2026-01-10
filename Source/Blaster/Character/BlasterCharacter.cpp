// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blaster/Blaster.h"
#include "Blaster/HUD/BlasterOverheadWidget.h"
#include "Blaster/Player/BlasterPlayerController.h"
#include "Blaster/Weapon/BlasterCombatComponent.h"
#include "Blaster/Weapon/BlasterWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidgetComponent->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UBlasterCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TurningInPlace = ETurningInPlace::NotTurning;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly)
	DOREPLIFETIME(ABlasterCharacter, Health)
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CombatComponent->BlasterCharacter = this;
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// on the server, pawns that are NOT controlled by the host
	// doesn't need check for Authority because PossessedBy is getting called only on server
	if (!IsLocallyControlled())
	{
		ShowPlayerName();
	}
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(DefaultMappingContext))
	{
		return;
	}

	if (const APlayerController* PlayerController{GetController<APlayerController>()})
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())})
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	BlasterPlayerController = Cast<ABlasterPlayerController>(Controller);
	
	if (IsLocallyControlled())
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
		// on the server, pawn that is controlled by the host
		if (HasAuthority())
		{
			ShowPlayerName();
		}
	}
}

void ABlasterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaSeconds);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaSeconds;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculatePitchAimOffset();
	}

	HideCameraIfCameraClose();
}

void ABlasterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// on the clients
	ShowPlayerName();
}

void ABlasterCharacter::ShowPlayerName() const
{
	const UBlasterOverheadWidget* BlasterOverheadWidget{Cast<UBlasterOverheadWidget>(OverheadWidgetComponent->GetUserWidgetObject())};
	if (!ensure(BlasterOverheadWidget))
	{
		return;
	}

	BlasterOverheadWidget->ShowPlayerName(this);
}

void ABlasterCharacter::OnRep_Health()
{
}

void ABlasterCharacter::HideCameraIfCameraClose() const
{
	if (!IsLocallyControlled())
	{
		return;
	}

	const float CameraToCharacterDistance = (CameraComponent->GetComponentLocation() - GetActorLocation()).Size();
	const bool bCharacterHidden = CameraToCharacterDistance < CameraThreshold;
	GetMesh()->SetVisibility(!bCharacterHidden);
	if (CombatComponent->EquippedWeapon)
	{
		CombatComponent->EquippedWeapon->GetWeaponMeshComponent()->bOwnerNoSee = bCharacterHidden;
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!ensure(MoveAction && LookAction && JumpAction && EquipAction && CrouchAction && AimAction && FireAction))
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent{Cast<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ABlasterCharacter::Equip);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::Crouch);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimStart);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimStop);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireStart);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireStop);
	}
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	if (!Value.IsNonZero())
	{
		return;
	}

	if (!GetController())
	{
		return;
	}

	// input is a Vector2D
	const FVector2D MovementVector{Value.Get<FVector2D>()};

	// find out which way is forward
	const FRotator Rotation{GetController()->GetControlRotation()};
	const FRotator YawRotation{0, Rotation.Yaw, 0};

	// get forward vector
	const FVector ForwardDirection{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)};

	// get right vector
	const FVector RightDirection{FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y)};

	// add movement
	// TODO change it to Internal_AddMovementInput
	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	const FVector2D LookAxisVector{Value.Get<FVector2D>()};

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::Equip()
{
	if (HasAuthority())
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
	else
	{
		Server_Equip();
	}
}

void ABlasterCharacter::Crouch()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		ACharacter::Crouch();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::AimStart()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::AimStop()
{
	if (CombatComponent)
	{
		CombatComponent->SetAiming(false);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::FireStart()
{
	if (CombatComponent)
	{
		CombatComponent->FireStart();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::FireStop()
{
	if (CombatComponent)
	{
		CombatComponent->FireStop();
	}
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(ABlasterWeaponBase* Weapon)
{
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(false);
		}
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const
{
	return CombatComponent && CombatComponent->EquippedWeapon;
}

bool ABlasterCharacter::IsAiming() const
{
	return CombatComponent && CombatComponent->bAiming;
}

ABlasterWeaponBase* ABlasterCharacter::GetEquippedWeapon() const
{
	if (!CombatComponent)
	{
		return nullptr;
	}

	return CombatComponent->EquippedWeapon;
}

void ABlasterCharacter::Multicast_Hit_Implementation()
{
	PlayHitReactMontage();
}

void ABlasterCharacter::PlayFireMontage(const bool bAiming) const
{
	if (!ensure(FireWeaponMontage))
	{
		return;
	}
	if (!CombatComponent->EquippedWeapon)
	{
		return;
	}

	UAnimInstance* AnimInstance{GetMesh()->GetAnimInstance()};
	AnimInstance->Montage_Play(FireWeaponMontage);
	const FName SectionName = FName(bAiming ? "RifleAim" : "RifleHip");
	AnimInstance->Montage_JumpToSection(SectionName);
}

void ABlasterCharacter::PlayHitReactMontage() const
{
	if (!ensure(HitReactMontage))
	{
		return;
	}
	if (!CombatComponent->EquippedWeapon)
	{
		return;
	}

	UAnimInstance* AnimInstance{GetMesh()->GetAnimInstance()};
	AnimInstance->Montage_Play(HitReactMontage);
	const FName SectionName{"FromFront"};
	AnimInstance->Montage_JumpToSection(SectionName);
}

FVector ABlasterCharacter::GetHitTargetLocation() const
{
	return CombatComponent->HitTargetLocation;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
// ReSharper disable once CppMemberFunctionMayBeConst
void ABlasterCharacter::OnRep_OverlappingWeapon(ABlasterWeaponBase* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void ABlasterCharacter::AimOffset(const float DeltaTime)
{
	if (!CombatComponent->EquippedWeapon)
	{
		return;
	}

	const float Speed{CalculateSpeed()};
	const bool bAirborne{GetCharacterMovement()->IsFalling()};

	// standing still, not jumping
	if (Speed == 0.f && !bAirborne)
	{
		bRotateRootBone = true;
		const FRotator CurrentAimRotation{FRotator(0.f, GetBaseAimRotation().Yaw, 0.f)};
		const FRotator DeltaAimRotation{UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation)};
		YawAimOffest = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::NotTurning)
		{
			InterpYawAimOffset = YawAimOffest;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	// running or jumping
	if (Speed > 0.f || bAirborne)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		YawAimOffest = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::NotTurning;
	}

	CalculatePitchAimOffset();
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (!CombatComponent->EquippedWeapon)
	{
		return;
	}

	bRotateRootBone = false;

	const float Speed{CalculateSpeed()};
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::NotTurning;
		}
		return;
	}

	TurningInPlace = ETurningInPlace::NotTurning;
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (YawAimOffest > 90.f)
	{
		TurningInPlace = ETurningInPlace::Right;
	}
	else if (YawAimOffest < -90.f)
	{
		TurningInPlace = ETurningInPlace::Left;
	}
	if (TurningInPlace != ETurningInPlace::NotTurning)
	{
		InterpYawAimOffset = FMath::FInterpTo(InterpYawAimOffset, 0.f, DeltaTime, 4.f);
		YawAimOffest = InterpYawAimOffset;
		if (FMath::Abs(YawAimOffest) < 15.f)
		{
			TurningInPlace = ETurningInPlace::NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::CalculatePitchAimOffset()
{
	PitchAimOffset = GetBaseAimRotation().Pitch;
	if (PitchAimOffset > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		PitchAimOffset = FMath::GetMappedRangeValueClamped(InRange, OutRange, PitchAimOffset);
	}
}

float ABlasterCharacter::CalculateSpeed() const
{
	FVector Velocity{GetVelocity()};
	Velocity.Z = 0.f;

	return Velocity.Size();
}

void ABlasterCharacter::Server_Equip_Implementation()
{
	CombatComponent->EquipWeapon(OverlappingWeapon);
}
