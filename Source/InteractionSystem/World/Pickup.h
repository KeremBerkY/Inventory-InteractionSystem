// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionSystem/InteractionSystemCharacter.h"
#include "InteractionSystem/Interfaces/InteractionInterface.h"
#include "Pickup.generated.h"

class UItemBase;

UCLASS()
class INTERACTIONSYSTEM_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------
	APickup();

	void InitializePickup(const int32 InQuantity);

	void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);

	FORCEINLINE UItemBase* GetItemData() { return ItemReference; };

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initilization")
	UDataTable* ItemDataTable;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initilization")
	FName DesiredItemID;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	UItemBase* ItemReference;
	
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initilization")
	int32 ItemQuantity;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction")
	FInteractableData InstanceInteractableData;
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------
	virtual void BeginPlay() override;
	
	virtual void Interact(AInteractionSystemCharacter* PlayerCharacter) override;
	void UpdateInteractableData();
	
	void TakePickup(const AInteractionSystemCharacter* Taker); // When we take pickup we need to be able to interface with the character's inventory.
	
#if WITH_EDITOR // this is purely for someone who's manipulating the game in the editor.
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; // this will essentially	store a reference to the last thing we tweak in the editor
#endif
	
};
