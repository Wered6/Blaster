// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "BlasterPlayerController.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats)
}

void ABlasterPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		OnPawnSet.AddDynamic(this, &ABlasterPlayerState::OnPawnInitialized);
	}
}

void ABlasterPlayerState::OnPawnInitialized(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (!NewPawn)
	{
		return;
	}

	BlasterCharacter = Cast<ABlasterCharacter>(NewPawn);
	if (BlasterCharacter->IsLocallyControlled())
	{
		BlasterPlayerController = BlasterCharacter->GetController<ABlasterPlayerController>();
	}
}

void ABlasterPlayerState::AddToScore(const float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	if (BlasterCharacter->IsLocallyControlled())
	{
		BlasterPlayerController->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (BlasterCharacter->IsLocallyControlled())
	{
		BlasterPlayerController->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToDefeats(const int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;

	if (BlasterCharacter->IsLocallyControlled())
	{
		BlasterPlayerController->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::OnRep_Defeats()
{
	if (BlasterCharacter->IsLocallyControlled())
	{
		BlasterPlayerController->SetHUDDefeats(Defeats);
	}
}
