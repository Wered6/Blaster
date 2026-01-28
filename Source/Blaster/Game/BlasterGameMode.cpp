// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/Player/BlasterPlayerController.h"
#include "Blaster/Player/BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle StartMatchTimerHandle;
	GetWorldTimerManager().SetTimer(
		StartMatchTimerHandle,
		[this]
		{
			StartMatch();
		},
		WarmupTime,
		false
	);
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayerController{Cast<ABlasterPlayerController>(*It)};
		BlasterPlayerController->SetMatchState(MatchState);
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter,
                                        ABlasterPlayerController* VictimController,
                                        ABlasterPlayerController* AttackerController) const
{
	ABlasterPlayerState* AttackerPlayerState{AttackerController->GetPlayerState<ABlasterPlayerState>()};
	ABlasterPlayerState* VictimPlayerState{VictimController->GetPlayerState<ABlasterPlayerState>()};

	// how is it even possible? why would we need this check?
	if (AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}
	VictimPlayerState->AddToDefeats(1);

	EliminatedCharacter->Eliminate();
}

void ABlasterGameMode::RequestRespawn(ACharacter* EliminatedCharacter, AController* VictimController)
{
	EliminatedCharacter->Reset();
	EliminatedCharacter->Destroy();

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	const int32 Selection{FMath::RandRange(0, PlayerStarts.Num() - 1)};
	RestartPlayerAtPlayerStart(VictimController, PlayerStarts[Selection]);
}
