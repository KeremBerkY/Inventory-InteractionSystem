// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "InteractionSystem/Components/InventoryComponent.h"
#include "InteractionSystem/Items/ItemBase.h"


APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = false; // he deleted the tick function, because he was set this to false

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	// InitializePickup(UItemBase::StaticClass(), ItemQuantity); // We are going to set the ID and set the Quantity and InitializePickup()
	InitializePickup(ItemQuantity);
}

void APickup::InitializePickup(const int32 InQuantity) // That will have give it the data table everything it needs you know. It will give this function everything it needs to get the item data table and spawn a new one 
{
	if (ItemDataTable && !DesiredItemID.IsNone()) // !DesiredItemID.IsNone(): this is an FName so, this is the way to check is it an empty string. It is not empty than ...
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, UItemBase::StaticClass());

		ItemReference->ID = ItemData->ID;
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->ItemQuality = ItemData->ItemQuality;
		ItemReference->NumericData = ItemData->NumericData;
		ItemReference->TextData = ItemData->TextData;
		ItemReference->AssetData = ItemData->AssetData;

		ItemReference->NumericData.bIsStackable = (ItemData->NumericData.MaxStackSize > 1) ? true : false;
		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity) // We already have the existing item. ItemToDrop. This function is not for dropping the item. The drop function is going to come from the player for now that's where the implementation of that is.
{
	ItemReference = ItemToDrop;
	
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity); // This is just a safety check
	ItemReference->NumericData.Weight = ItemToDrop->GetItemSingleWeight();
	ItemReference->OwningInventory = nullptr;
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);
	
	UpdateInteractableData();

}

void APickup::UpdateInteractableData() // Any time something changes with the item, with the pickup. You just call this.
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->TextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->TextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData; // This is the one that the interface itself sees which can then flow through over to our interaction widget
}

void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact(AInteractionSystemCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(const AInteractionSystemCharacter* Taker)
{
	if (!IsPendingKillPending()) // Check is this item in the process of being deleted. They might run this pickup the item and then it tries to run it again, like spamming E key...
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData(); // the pickup quantities updated this updates the interactable data of the pickup 
					Taker->UpdateInteractionWidget(); // this one actually updates it on the UI that player sees
					break;
				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				}

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null!"));

			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal item reference was somehow null!"));

		}
	}
}

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None; // PropertyChangedEvent.Property = The actual property that changed. // So, get the name from whatever changed 

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID)) // kind of checks validity of things keeps it safe. // Does that match the DesiredItemID in the Pickup class the thing that was changed.
	{
		if (ItemDataTable)
		{
			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString())) // If it does try to get resulting item from the DataTable.
			{
				PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh); // And set the Mesh of the pickup to that mesh.
			}
		}
	}
}
