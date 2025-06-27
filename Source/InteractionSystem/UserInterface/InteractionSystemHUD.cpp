
#include "InteractionSystemHUD.h"
#include "InteractionSystem/UserInterface/MainMenu.h"
#include "InteractionSystem/UserInterface/Interaction/InteractionWidget.h"

AInteractionSystemHUD::AInteractionSystemHUD()
{
}

void AInteractionSystemHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass); // pickup örneğiyle verelim. pickup bütün datasını data table dan alıyor editörden değil pickup da static class kullanabilirsin ama widgetlarda bu şekilde yapmak zorundasın.
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
		CrosshairWidget->AddToViewport();
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AInteractionSystemHUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AInteractionSystemHUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AInteractionSystemHUD::ToggleMenu()
{
	if (bIsMenuVisible)
	{
		HideMenu();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();

		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AInteractionSystemHUD::ShowCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
	
}

void AInteractionSystemHUD::HideCrosshair()
{
	if (CrosshairWidget)
		CrosshairWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void AInteractionSystemHUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AInteractionSystemHUD::HideInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AInteractionSystemHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const // If i am aiming new interactable i'm calling this function. i might as well show the interact show the widget.
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}


