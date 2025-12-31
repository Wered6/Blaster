// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blaster/HUD/BlasterOverheadWidget.h"
#include "Blaster/Weapons/BlasterCombatComponent.h"
#include "Blaster/Weapons/BlasterWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"


ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetMesh());
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidgetComponent->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UBlasterCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly)
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CombatComponent->BlasterCharacter = this;
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

	// on the server, pawn that is controlled by the host
	if (HasAuthority() && IsLocallyControlled())
	{
		ShowPlayerName();
	}
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// on the server, pawns that are NOT controlled by the host
	// doesn't need check for Authority because PossessBy is getting called only on server
	if (!IsLocallyControlled())
	{
		ShowPlayerName();
	}
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

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!ensure(MoveAction && LookAction && JumpAction && EquipAction))
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent{Cast<UEnhancedInputComponent>(PlayerInputComponent)})
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ABlasterCharacter::Equip);
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

void ABlasterCharacter::Server_Equip_Implementation()
{
	CombatComponent->EquipWeapon(OverlappingWeapon);
}
