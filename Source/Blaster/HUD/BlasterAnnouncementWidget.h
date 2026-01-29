// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlasterAnnouncementWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnnouncementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetWarmupTimeText(const float CountdownTime) const;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> WarmupTimeText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> InfoText;
};
