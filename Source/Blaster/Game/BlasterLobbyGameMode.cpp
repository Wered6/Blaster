// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ABlasterLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const int32 PlayerCount{GameState->PlayerArray.Num()};
	if (PlayerCount == NumberOfPlayers)
	{
		bUseSeamlessTravel = true;
		GetWorld()->ServerTravel(FString("/Game/Blaster/Map/Lvl_Blaster?listen"));
	}
}
