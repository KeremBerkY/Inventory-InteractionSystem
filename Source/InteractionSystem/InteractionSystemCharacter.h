// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "Logging/LogMacros.h"
#include "InteractionSystemCharacter.generated.h"

class UTimelineComponent;
class UItemBase;
class UInventoryComponent;
class AInteractionSystemHUD;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(.0f) {};
	
	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
	
};

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AInteractionSystemCharacter : public ACharacter
{
	GENERATED_BODY()
	

public:
	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	bool bAiming;
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------
	AInteractionSystemCharacter();
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); }

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; }

	void UpdateInteractionWidget() const;

	void DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop);
	
protected:
	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	UPROPERTY()
	AInteractionSystemHUD* HUD;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	
	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable; // TScriptInterface<IInteractionInterface> that's holding the reference to whatever interactable we are looking at

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	UInventoryComponent* PlayerInventory;

	// Interaction properties
	float InteractionCheckFrequency;
	float InteractionCheckDistance;
	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;

	// timeline properties used for camera aiming transition
	UPROPERTY(VisibleAnywhere, Category = "Character | Camera")
	FVector DefaultCameraLocation;
	UPROPERTY(VisibleAnywhere, Category = "Character | Camera")
	FVector AimingCameraLocation;

	TObjectPtr<UTimelineComponent> AimingCameraTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Character | Aim Timeline")
	UCurveFloat* AimingCameraCurve;
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void ToggleMenu();

	void Aim();
	void StopAiming();
	UFUNCTION()
	void UpdateCameraTimeline(const float TimelineValue) const; // this is like timeline tick
	UFUNCTION()
	void CameraTimelineEnd(); // this is when the timeline reaches the end
	
	void PerformInteractionCheck(); // Her tick de çalışacak ve line trace shoot edecek bir interaction check
	void FoundInteractable(AActor* NewInteractable); // yukarıdaki ile check ettik eğer interactable implement etmişse bul
	void NoInteractableFound(); // bu da interactable implement etmemişse veya hiç bulunamadıysa

	// Bunlar Interface'dekinden farklı bunlar main one. Karakterin kendisinden geliyor. Örn bu Interact() acting, Interactable'daki Interact() receiving that action. Buradakiyle interfacedekini çağrıyoruz.
	void BeginInteract();
	void EndInteract();
	void Interact();
	
};

