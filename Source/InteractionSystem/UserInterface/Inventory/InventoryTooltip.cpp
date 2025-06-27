// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryTooltip.h"

#include "InventoryItemSlot.h"
#include "Components/TextBlock.h"
#include "InteractionSystem/Items/ItemBase.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	switch (ItemBeingHovered->ItemType)
	{
	case EItemType::Armor:
		break;
	case EItemType::Weapon:
		break;
	case EItemType::Shield:
		break;
	case EItemType::Spell:
		break;
	case EItemType::Consumable:
		ItemType->SetText(FText::FromString("Consumable"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Quest:
		break;
	case EItemType::Mundane:
		ItemType->SetText(FText::FromString("Mundane Item"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
		UsageText->SetVisibility(ESlateVisibility::Collapsed);
		break;
	}

	ItemName->SetText(ItemBeingHovered->TextData.Name);
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
	ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
	UsageText->SetText(ItemBeingHovered->TextData.UsageText);
	ItemDescription->SetText(ItemBeingHovered->TextData.Description);
	// MaxStackSizeText->SetText(FText::FromString("Max Stack Size: "));
	// StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));
	
	const FString WeightInfo =
		{"Weight: " + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight())};

	StackWeight->SetText(FText::FromString(WeightInfo));

	if (ItemBeingHovered->NumericData.bIsStackable)
	{
		const FString StackInfo =
			{"Max stack size: " + FString::FromInt(ItemBeingHovered->NumericData.MaxStackSize)};

		MaxStackSize->SetText(FText::FromString(StackInfo));
	}
	else
	{
		MaxStackSize->SetVisibility(ESlateVisibility::Collapsed); // If it's none stackable
		// MaxStackSizeText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
