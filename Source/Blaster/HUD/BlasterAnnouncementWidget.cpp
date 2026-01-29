// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnnouncementWidget.h"
#include "Components/TextBlock.h"


void UBlasterAnnouncementWidget::SetWarmupTimeText(const float CountdownTime) const
{
	if (!ensure(WarmupTimeText))
	{
		return;
	}

	if (CountdownTime < 0.f)
	{
		WarmupTimeText->SetText(FText());
		return;
	}

	const int32 TotalSeconds{FMath::FloorToInt(CountdownTime)};
	const int32 Minutes{TotalSeconds / 60};
	const int32 Seconds{TotalSeconds % 60};

	FNumberFormattingOptions Options;
	Options.SetMinimumIntegralDigits(2);

	WarmupTimeText->SetText(FText::Format(FText::FromString(TEXT("{0}:{1}")),
	                                      FText::AsNumber(Minutes, &Options),
	                                      FText::AsNumber(Seconds, &Options)));
}

void UBlasterAnnouncementWidget::SetAnnouncementText(const FString& NewAnnouncementText) const
{
	if (!ensure(AnnouncementText))
	{
		return;
	}

	AnnouncementText->SetText(FText::FromString(NewAnnouncementText));
}

void UBlasterAnnouncementWidget::SetInfoText(const FString& NewInfoText) const
{
	if (!ensure(InfoText))
	{
		return;
	}

	InfoText->SetText(FText::FromString(NewInfoText));
}
