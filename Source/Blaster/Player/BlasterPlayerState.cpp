// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "BlasterPlayerController.h"
#include "Blaster/Character/BlasterCharacter.h"

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
