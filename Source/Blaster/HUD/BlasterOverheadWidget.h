// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BlasterOverheadWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeDestruct() override;
	
public:
	void SetDisplayText(const FString& TextToDisplay) const;
	UFUNCTION(BlueprintCallable, Category="Blaster")
	void ShowPlayerNetRole(const APawn* InPawn) const;
	UFUNCTION(BlueprintCallable, Category="Blaster")
	void ShowPlayerName(const APawn* InPawn) const;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DisplayText;
};
