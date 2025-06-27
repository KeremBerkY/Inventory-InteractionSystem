// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "InteractionSystem/InteractionSystemCharacter.h"
#include "InteractionSystem/Items/ItemBase.h"
#include "Inventory/ItemDragDropOperation.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AInteractionSystemCharacter>(GetOwningPlayerPawn()); // Player Reference alıyoruz UE VScode notlarımda Widget bölümünde açıklama var!
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;
}
