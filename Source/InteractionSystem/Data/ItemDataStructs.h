#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemDataStructs.generated.h"

UENUM()
enum class EItemQuality : uint8 // Epic, Rare, Legendary ... Tarzı şeyler yazabilirsin.
{
	Shoddy UMETA(DisplayName = "Shoddy"),
	Common UMETA(DisplayName = "Common"), 
	Quality UMETA(DisplayName = "Quality"),
	Masterwork UMETA(DisplayName = "Masterwork"),
	Grandmaster UMETA(DisplayName = "Grandmaster")
};

UENUM()
enum class EItemType : uint8
{
	Armor UMETA(DisplayName = "Armor"),
	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Spell UMETA(DisplayName = "Spell"),
	Consumable UMETA(DisplayName = "Consumable"),
	Quest UMETA(DisplayName = "Quest"),
	Mundane UMETA(DisplayName = "Mundane")
};

USTRUCT()
struct FItemStatistics // Item bonus gibi düşün burayı giydin +2 bonus güç verdi gibi
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float ArmorRating;
	
	UPROPERTY(EditAnywhere)
	float DamageValue;
	
	UPROPERTY(EditAnywhere)
	float RestorationAmount;
	
	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT()
struct FItemTextData // UI için gerekli
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	FText Name;
	
	UPROPERTY(EditAnywhere)
	FText Description;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText; // mesela pickup yazısı gibi üzerine geldin o çıktı
	
	UPROPERTY(EditAnywhere)
	FText UsageText; // Potion güzel bir örnek. Inventorye girdin, Drink veya Use diyecek. 
};

USTRUCT()
struct FItemNumericData // weight, maximum stack size gibi 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;
	
	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;
	
	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT()
struct FItemAssetData // Item soundlarını da buraya yazabilirsin. Her Itemın sesi olabilir alındığında falan
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;
	
};

USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;
	
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemTextData TextData;

	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemNumericData NumericData;
	
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemAssetData AssetData;
	
};
