# InteractionSystem

Developed with Unreal Engine 5

# 🧾 Advanced Inventory & Interaction System – Unreal Engine 5 (C++)

## 🎯 Project Summary (For CV & GitHub)

An advanced, modular, and extensible Inventory & Interaction system developed in C++ for Unreal Engine 5. The player can interact with nearby objects (pickups, doors, switches) and collect items into an inventory governed by weight and slot limitations. The UI supports drag & drop, tooltips, and a HUD-driven menu system. The architecture is built around Unreal’s native features like `Delegates`, `Interfaces`, `UObject`, `UComponent`, and `UserWidget`, closely following an MVC-like structure.

---

## 🧱 Architecture Overview

### 🔷 Core Components:

* **InventoryComponent**: Manages item stacks, weight/slot limits, add/remove logic
* **ItemBase**: Core UObject representing item data and state
* **InteractionInterface**: Interface implemented by all interactable world actors
* **InteractionSystemCharacter**: The player character class – handles raycasts, inventory access, and interactions
* **InteractionSystemHUD**: Central UI controller – handles menus, crosshairs, and interaction widgets

### 🖼️ UI Components:

* `UInventoryPanel`: Displays all items in a grid layout
* `UInventoryItemSlot`: Represents each item visually and supports drag operations
* `UInventoryTooltip`: Displays detailed item data on hover
* `UDragItemVisual`: Shown while dragging an item
* `UMainMenu`: Handles dropping items from inventory into the world
* `UInteractionWidget`: Displays interaction name/action/quantity/progress when looking at objects

---

## 🧠 System Highlights

| Feature                             | Description                                                                    |
| ----------------------------------- | ------------------------------------------------------------------------------ |
| 🎒 Weight & Slot Limited Inventory  | Enforces carrying constraints using `WeightCapacity` and `SlotCapacity`        |
| 🔄 Delegate-Driven Updates          | Uses `OnInventoryUpdated` to auto-refresh UI elements                          |
| 🔁 Full Drag & Drop Support         | Items can be dragged across slots, dropped into menu, or outside the inventory |
| 📦 Stackable & Non-Stackable Items  | Dynamically handles stack limits and partial stacks                            |
| 🧩 Tooltip System                   | Displays item stats, usage, and description on hover                           |
| 🎮 Centralized HUD UI Control       | Manages visibility and updates of all UI widgets                               |
| 🧭 Raycast-Based Interaction System | Character uses line tracing to detect interactables                            |
| 🛠️ DataTable-Driven Item Design    | Items are managed through structured `FItemData` entries                       |

---

## 📊 Flow Diagrams

### 1. **Interaction & Pickup Flow**

```
Tick() 
 └──> PerformInteractionCheck()
       ├──> FoundInteractable()
       │     └──> HUD->UpdateInteractionWidget()
       └──> NoInteractableFound()

[Press E] 
 └──> BeginInteract()
       └──> Target->Interact()
             └──> InventoryComponent->HandleAddItem()
                   └──> Broadcast: OnInventoryUpdated → UI Refresh
```

### 2. **Inventory Refresh Flow**

```
OnInventoryUpdated.Broadcast()
 └──> InventoryPanel::RefreshInventory()
       └──> Create Slot (InventoryItemSlot)
             └──> Add Tooltip & Drag Support
```

---

## 🔧 Simplified Class Diagram

```
InteractionSystemCharacter
├── InventoryComponent
│   └── ItemBase (TArray)
├── TScriptInterface<IInteractionInterface>
├── InteractionSystemHUD
│   ├── InteractionWidget
│   ├── MainMenu
│   └── CrosshairWidget

InventoryPanel
└── InventoryItemSlot
    └── InventoryTooltip
    └── DragItemVisual

ItemDragDropOperation → SourceItem + SourceInventory
```

---

## 📘 Core Unreal Engine Features Used

* `UActorComponent`, `UObject`, `UserWidget`, `AHUD`
* `TScriptInterface`, `TSubclassOf`, `FDelegate`
* `UENUM`, `USTRUCT`, `DataTable`, `BlueprintReadOnly`
* `LineTrace`, `Enhanced Input`, `TimelineComponent`

---

## 🎥 Demo Video

Watch the gameplay demo here:  
👉 [Advanced Inventory & Interaction System | UE5 C++ Project Showcase](https://youtu.be/7GdF9QAjjiQ)

---

## 🔍 Installation & Usage

### Requirements:

* Unreal Engine 5.3.2
* Enhanced Input (plugin)

### Setup Instructions:

1. Clone the repository:

```bash
git clone https://github.com/KeremBerkY/Inventory-InteractionSystem
```

2. Open the `.uproject` file
3. Right-click and select **Generate Visual Studio project files**
4. Compile the project and click **Play** in the editor

---

## 📎 Extensibility Note

> This system is designed to be easily extended with modules like weapon equipping, backpack expansion, container looting (chests, crates), or crafting systems.

---

## ⚠️ License & Usage

This project is shared for portfolio purposes only. Not intended for commercial use.

---

Signature: Advanced Inventory & UI System - Kerem Berk Yümlü
