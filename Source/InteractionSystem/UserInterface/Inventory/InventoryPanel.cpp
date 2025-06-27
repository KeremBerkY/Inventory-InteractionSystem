// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryPanel.h"
// game
#include "InventoryItemSlot.h"
#include "InteractionSystem/InteractionSystemCharacter.h"
#include "InteractionSystem/Components/InventoryComponent.h"

// engine
#include "ItemDragDropOperation.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "InteractionSystem/Items/ItemBase.h"

void UInventoryPanel::NativeOnInitialized() // We set up all references here when it's initialized. So, that way it knows where it's pulling items from.
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AInteractionSystemCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory); // We bind our updated delegate so, that anytime the inventory changes we call refresh
			SetInfoText(); // When everything is initialized. If the inventory has something in it already you want to be able to see that reflected. Otherwise it's going to get reflected later when the firs time RefreshInventory is called
		}
	}
}

void UInventoryPanel::SetInfoText() const
{
	const FString WeightInfoValue{
		FString::SanitizeFloat(InventoryReference->GetInvetoryTotalWeight()) + "/"
		+ FString::SanitizeFloat(InventoryReference->GetWeightCapacity())
	};

	const FString CapacityInfoValue{
		FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
		+ FString::FromInt(InventoryReference->GetSlotCapacity())
	};

	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

void UInventoryPanel::RefreshInventory() // this is the function that's going to make everything show up in the inventory
{
	if (InventoryReference && InventorySlotClass)
	{
		InventoryWrapBox->ClearChildren();
		
		for (UItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);

			InventoryWrapBox->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}


bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InventoryPanel."))

		// returning true will stop the drop operation at this widget
		return true;
	}
	// returning false will cause the drop operation to fall through to underlying widgets (if any)
	return false;
	
}
