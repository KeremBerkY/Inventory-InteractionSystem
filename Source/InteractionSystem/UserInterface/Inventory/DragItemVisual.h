// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DragItemVisual.generated.h"

class UTextBlock;
class UImage;
class UBorder;
/**
 * 
 */
UCLASS()
class INTERACTIONSYSTEM_API UDragItemVisual : public UUserWidget
{
	GENERATED_BODY()

public:
		
	UPROPERTY(VisibleAnywhere, Category = "Drag item visual", meta=(BindWidget))
	UBorder* ItemBorder;
	
	UPROPERTY(VisibleAnywhere, Category = "Drag item visual", meta=(BindWidget))
	UImage* ItemIcon;
	
	UPROPERTY(VisibleAnywhere, Category = "Drag item visual", meta=(BindWidget))
	UTextBlock* ItemQuantity;
};
