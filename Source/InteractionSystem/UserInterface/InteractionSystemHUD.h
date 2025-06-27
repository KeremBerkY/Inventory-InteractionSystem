// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InteractionSystemHUD.generated.h"

struct FInteractableData;
class UMainMenu;
class UInteractionWidget;

/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API AInteractionSystemHUD : public AHUD // The HUD is the controller for our UI elements
{
	GENERATED_BODY()

public:
	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	bool bIsMenuVisible; // aim mode'dayım menüye geçiş yaptım. Bu aim modunu sürdürmek istemiyorum. Ne zaman (true) oldu. Karaktere aimi durdurmasını söylüyorum. Menü görülür ve bir container lootlayacaksın, containerı getir ve menüyü kapatma gibi şeyler eklenecek...
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------
	AInteractionSystemHUD();

	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	void ShowCrosshair();
	void HideCrosshair();

	void ShowInteractionWidget() const; // Bu diğer sınıfların özellikle karakter sınıfının widgeta baktığı zaman.
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const; // referans yapmadık çünkü her çağrıldığında kopyasını istemiyoruz. Pointer ile target Interactableda ne varsa referans ile gönderebilir.
	
protected:

	//---------------------------------------------------------------------------------------------
	// PROPERTIES & VARIABLES
	//---------------------------------------------------------------------------------------------
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UUserWidget* CrosshairWidget;
	
	//---------------------------------------------------------------------------------------------
	// FUNCTIONS
	//---------------------------------------------------------------------------------------------

	virtual void BeginPlay() override;

};
