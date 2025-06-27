// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UProgressBar;
class UTextBlock;
struct FInteractableData;
class AInteractionSystemCharacter;
/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Interaction Widget | Player Reference")
	AInteractionSystemCharacter* PlayerReference;

	void UpdateWidget(const FInteractableData* InteractableData) const; // InteractableData'yı alıcaz. İçerisinde textler'i barındırıyorum. Bu text'leri farklı text bloklarını update etmek için kullanıcaz.

protected:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* NameText;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* ActionText;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* QuantityText; // When we start looking at items that are laying on the ground in the form of pickup class.

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UTextBlock* KeyPressText; // hold instead of press gibi yazılar.

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	UProgressBar* InteractionProgressBar; // used for timed Interactables.
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction Widget | Interactable Data")
	float CurrentInteractionDuration;

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	float UpdateInteractionProgress(); // Progress bar için kullanıcaz. returns a float and this is how the progress bar knows how much to fill up.
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
