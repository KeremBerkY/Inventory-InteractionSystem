// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "InteractionSystem/Items/ItemBase.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn)) // Contains() is a quick function. Checks if the array contains the element returns true.
		{
			return ItemIn;
		}
	}

	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn)) // You have your array and then you have a scope you know an access operator :: thats going to get the type of element in the array, and it's going to make it a pointer, and we call that result
		{
			return *Result;
		}
	}

	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = // we want a generic way to access the type of the array in this case UItemBase. We are getting pointer to that type ElementType* so we have a double pointer.
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem)  // We are calling FindByPredicate() predicate is just a fancy word that means a condition right. It's something checks a condition. In this case it is essentially our predicate is really just logical check to say return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
		{
			return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
		}))
	{
		return *Result;
	}

	return nullptr;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount) // yığına eklenecek item sayısının, envanterin ağırlık kapasitesini aşmadan eklenebilir olup olmadığını kontrol eder. Bu, item'lerin ağırlığı ve envanter kapasitesi dikkate alınarak hesaplanır.
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight()); // If this was six but, we only were trying to add three. You can fully add three
	if (WeightMaxAddAmount >= RequestedAddAmount )
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount) //  mevcut item yığınına (existing stack) kaç tane item ekleyerek o yığını tamamen doldurabileceğinizi hesaplar. Örneğin, bir yığının maksimum kapasitesi 10 item ise ve yığında 6 item varsa, bu fonksiyon 4 dönecektir, yani bu yığına 4 item ekleyerek yığın tam dolu hale getirilebilir.
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;
	return FMath::Min(InitialRequestedAddAmount, AddAmountToMakeFullStack);
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast(); // When you subscribe to it in other classes you know once this is called they will immediately get notified and we basically take the delegate in other class and bind a function to it. So, what you really doing is when you call this broadcast you execute functions in other classes.
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);

	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSingleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		// We have a stack of three potions. We want to split that into a stack of two and one so, this would be a one(AmountToSplit), this would be the potion reference itself(ItemIn). All you're doing at that point is removing some and then adding a copy that with the new amount. 
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
		
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* InputItem)
{
	// Check if in the input item has valid weight
	if (FMath::IsNearlyZero(InputItem->GetItemStackWeight()) || InputItem->GetItemStackWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item has valid weight value."), InputItem->TextData.Name));
	}

	// will the item weight overflow weight capacity
	if (InventoryTotalWeight + InputItem->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. Item would overflow weight limit."), InputItem->TextData.Name));
	}

	// adding one more item would overflow slot capacity
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the inventory. All inventory slots are full."), InputItem->TextData.Name));
	}

	// AddNewItem(InputItem, RequestedAddAmount);
	AddNewItem(InputItem, 1);
	return FItemAddResult::AddedAll(1, FText::Format(
			FText::FromString("Successfully added a single {0} to the inventory."), InputItem->TextData.Name));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		// invalid item data
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;

	// check if the input item already exists in the inventory and is not a full stack
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn); // All it's going to do is look for the next partial stack that matches that item type // FindNextPartialStack fonksiyonu, envanterde henüz tamamen dolmamış (partial) ve aynı türde item'leri içeren bir yığın bulur. Bu yığın, ItemIn ile aynı item türüne sahiptir. Eğer böyle bir yığın bulunursa ExistingItem değişkenine atanır, aksi takdirde nullptr olur.

	// distribute item stack over existing stacks
	while (ExistingItemStack) // this way anything that when it finds the existing stacks it'll put it into those // Döngü, mevcut bir partial stack (kısmen dolu yığın) bulunduğu sürece çalışır. Her iterasyonda bir yığının içerisine mümkün olduğunca fazla item eklemeye çalışır.
	{
		// calculate how many of the existing item would be needed to make the next full stack 
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute); // it's going to say okay you know this stack is max of four but, you only have two in it. So, this would come back as a two
		// calculate how many of the AmountToMakeFullStack can actually be carried based on weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		// As long as the remaining amount of the item does not overflow weight capacity
		if (WeightLimitAddAmount > 0)
		{
			// adjust the existing item stack quantity and inventory total weight 
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);

			// adjust the count to be distributed 
			AmountToDistribute -= WeightLimitAddAmount;
			
			ItemIn->SetQuantity(AmountToDistribute);

			// TODO: Refine this logic since going over weight capacity should not ever be possible
			// if max weight capacity is reached, no need to run the loop again
			if (InventoryTotalWeight + ExistingItemStack->GetItemSingleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute; // this show you the amount you actually added
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				// this block will be reached if distributing an item across multiple stacks
				// and the weight limit is hit during that process
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			return 0;
		}
		// above⬆️is only a partial amount was added

		// this one assumes distributed everything
		if (AmountToDistribute <= 0)
		{
			// all of the input item was distributed across existing stacks 
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// check if there is still another valid partial stack of the input item 
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}

	// no more partial stack found, check if a new stack can be added
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// attempt to add as many from the remaining item quantity that can fit inventory weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// if there is still more item to distribute, but weight limit has been reached
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				// adjust the input item and add a new stack with as many as can be held 
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				// create a copy since only a partial stack is being added
				AddNewItem(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// otherwise, the full remainder of the stack can be added
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}
	}

	OnInventoryUpdated.Broadcast();
	return RequestedAddAmount -	AmountToDistribute;
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		// Handle non-stackable items
		if (!InputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem);
		}

		// Handle stackable
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				InitialRequestedAddAmount,
				InputItem->TextData.Name));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				InputItem->TextData.Name,
				StackableAmountAdded));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				InputItem->TextData.Name));
		}
	}

	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed"));
}

void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup) // We don't create a copy if it's pickup because this(bIsPickup) assumes it's on the ground. It's something that was put into the world and initialized independently and it's going to get destroyed once we pick it up. So, in that case we don't need to create a brand new one. // If it's already a copy of another item that means we've already called create copy somewhere else so we don't need to copy it again.
	{
		// If the item is already a copy, or is a world pickup
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else // but if it's not already a copy and it's not coming from the world that means it's coming from somewhere else and it needs to be a separate memory reference.
	{
		// used when splitting or draggging to/from another inventory
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}


