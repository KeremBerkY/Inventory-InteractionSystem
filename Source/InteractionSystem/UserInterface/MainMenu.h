// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class AInteractionSystemCharacter;
/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AInteractionSystemCharacter* PlayerCharacter;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	
};
