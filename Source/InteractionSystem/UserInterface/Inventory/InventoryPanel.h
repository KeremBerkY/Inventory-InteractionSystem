// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryItemSlot;
class UInventoryComponent;
class AInteractionSystemCharacter;
class UTextBlock;
class UWrapBox;
/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION() // this is going to be bound tı a delegate it has to be a UFUNCTION. You'll run into weird errors that are not always super intuitive if it's not a UFUNCTION. If you forget and get some errors, you know that's something to check.
	void RefreshInventory();

	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryWrapBox;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeightInfo;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	AInteractionSystemCharacter* PlayerCharacter; // the reason we need this is because we're getting the inventory reference from the PlayerCharacter who's interacting with something. So, we'll be able to use this to say get inventory contents and show the array.
	UPROPERTY()
	UInventoryComponent* InventoryReference;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass; // because we're setting this. This widget is going to be responsible for creating all the new item slot classes when things are added. So, we're going to set this in the actual inventory panel.
															// the drag itself though is handled from the InventorySlotClass. So, because you're going to click on an inventory item not the panel itself. So, that's why there's no drag stuff
protected:
	void SetInfoText() const;

	// Reason i want those is because you can drop things in to the inventory panel so we need to handle that
	virtual void NativeOnInitialized() override; // will just let us set some stuff when it's created. This is almost like the constructor
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
};
