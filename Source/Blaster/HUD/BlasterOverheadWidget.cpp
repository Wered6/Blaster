// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterOverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UBlasterOverheadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}

void UBlasterOverheadWidget::SetDisplayText(const FString& TextToDisplay) const
{
	if (!ensure(DisplayText))
	{
		return;
	}

	DisplayText->SetText(FText::FromString(TextToDisplay));
}

void UBlasterOverheadWidget::ShowPlayerNetRole(const APawn* InPawn) const
{
	const ENetRole RemoteRole{InPawn->GetRemoteRole()};
	FString Role;
	switch (RemoteRole)
	{
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	case ROLE_MAX:
		Role = FString("MAX");
		break;
	}
	const FString RemoteRoleString{FString::Printf(TEXT("Remote Role: %s"), *Role)};
	SetDisplayText(RemoteRoleString);
}

void UBlasterOverheadWidget::ShowPlayerName(const APawn* InPawn) const
{
	const APlayerState* PlayerState{InPawn->GetPlayerState()};
	if (!ensure(PlayerState))
	{
		return;
	}
	
	const FString PlayerName{PlayerState->GetPlayerName()};
	SetDisplayText(PlayerName);
}
