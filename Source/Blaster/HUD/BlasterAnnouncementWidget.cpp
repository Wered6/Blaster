// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnnouncementWidget.h"
#include "Components/TextBlock.h"


void UBlasterAnnouncementWidget::SetWarmupTimeText(const float CountdownTime) const
{
	if (!ensure(WarmupTimeText))
	{
		return;
	}

	const int32 TotalSeconds{FMath::Max(0, FMath::FloorToInt(CountdownTime))};
	const int32 Minutes{TotalSeconds / 60};
	const int32 Seconds{TotalSeconds % 60};

	FNumberFormattingOptions Options;
	Options.SetMinimumIntegralDigits(2);

	WarmupTimeText->SetText(FText::Format(FText::FromString(TEXT("{0}:{1}")),
	                                      FText::AsNumber(Minutes, &Options),
	                                      FText::AsNumber(Seconds, &Options)));
}
