// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class AInteractionSystemCharacter;

UENUM()
enum class EInteractableType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"), // door, gate, window
	Toggle UMETA(DisplayName = "Toggle"), // button, switch, activate some functionality
	Container UMETA(DisplayName = "Container"),
};

USTRUCT()
struct FInteractableData  // for anything inherits this interface, It's going to get its own copy of FInteractableData that set according to that instance. So all of these should be EditInstanceOnly
{
	GENERATED_USTRUCT_BODY()

	// The Reason we are writing GetEmpty() or 0(zero) is, we are going to override all of this and set these values. There is no reason to do that right now
	FInteractableData() : InteractableType(EInteractableType::Pickup), Name(FText::GetEmpty()),
			Action(FText::GetEmpty()), Quantity(0), InteractionDuration(0.f) {};

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;
	
	UPROPERTY(EditInstanceOnly)
	FText Name;
	
	UPROPERTY(EditInstanceOnly)
	FText Action;
	
	// Used only for pickups
	UPROPERTY(EditInstanceOnly) 
	int8 Quantity; // Blueprintler int32 kullanıyor bir şey farketmez istersen int32 yaz

	// Used for things like valves, doors, etc. that require an interaction timer
	UPROPERTY(EditInstanceOnly) 
	float InteractionDuration;
	
};

// This class does not need to be modified.
UINTERFACE()
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Aşağıya yazıyorsun ne varsa. Effect, Sparkle, pop-up smth, faint glow ne ararsan buraya implement edebilirsin.
 */
class INTERACTIONSYSTEM_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(AInteractionSystemCharacter* PlayerCharacter); // Interact ettiğimiz şey bizim karakter olduğumuzu bilsin.

	FInteractableData InteractableData;	
};
