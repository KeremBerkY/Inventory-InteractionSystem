
// Game
#include "InteractionSystemCharacter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UserInterface/InteractionSystemHUD.h"
#include "Components/InventoryComponent.h"

//Engine
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "World/Pickup.h"

// Helper
#include "DrawDebugHelpers.h"
#include "Components/TimelineComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AInteractionSystemCharacter

AInteractionSystemCharacter::AInteractionSystemCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.0f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AimingCameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AimingCameraTimeline"));
	DefaultCameraLocation = FVector{0.f, 0.f, 65.f};
	AimingCameraLocation = FVector{175.f, 50.f, 55.f};
	CameraBoom->SocketOffset = DefaultCameraLocation;
	
	InteractionCheckFrequency = .1f;
	InteractionCheckDistance = 200.0f;

	// capsule default dimensions = 34.f, 88.f
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	BaseEyeHeight = 76.0f;
}

void AInteractionSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInteractionSystemCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AInteractionSystemCharacter::Look);

		// Interacting
		PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AInteractionSystemCharacter::BeginInteract);
		PlayerInputComponent->BindAction("Interact", IE_Released, this, &AInteractionSystemCharacter::EndInteract);

		// CameraAim
		PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AInteractionSystemCharacter::Aim);
		PlayerInputComponent->BindAction("Aim", IE_Released, this, &AInteractionSystemCharacter::StopAiming);

		// Menu
		PlayerInputComponent->BindAction("ToggleMenu", IE_Pressed, this, &AInteractionSystemCharacter::ToggleMenu);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AInteractionSystemCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AInteractionSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AInteractionSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	HUD = Cast<AInteractionSystemHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	// ChatGPT Sorular 7 açıklaması!
	FOnTimelineFloat AimLerpAlphaValue;
	FOnTimelineEvent TimelineFinishedEvent;
	AimLerpAlphaValue.BindUFunction(this, FName("UpdateCameraTimeline"));
	TimelineFinishedEvent.BindUFunction(this, FName("CameraTimelineEnd"));

	if (AimingCameraTimeline && AimingCameraCurve)
	{
		AimingCameraTimeline->AddInterpFloat(AimingCameraCurve, AimLerpAlphaValue);
		AimingCameraTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent);
	}
}

void AInteractionSystemCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
}

void AInteractionSystemCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{FVector::ZeroVector};

	if (!bAiming)
	{
		InteractionCheckDistance = 200.f;
		TraceStart = GetPawnViewLocation();

	}
	else
	{
		InteractionCheckDistance = 350.f;
		TraceStart = FollowCamera->GetComponentLocation();
	}
	
	FVector TraceEnd{TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance)};

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		// DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0.f,2.0f);
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass())) // StaticClass(): return c++ class itself demek
			{
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable) // Yeni bir objeye raycast atıyoruz CurrentInteractable'dan ayrıldık.
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable) // Zaten hala onunla etkileşim halindeyiz demek raycast ona atıyor.
				{
					return;
				}
			}
		}
	}
	
	NoInteractableFound();
	
}

void AInteractionSystemCharacter::FoundInteractable(AActor* NewInteractable) // bu her saniye check edilen bir şey NewInteractable diye verdiğimiz datayı karakter kafasını çevirdiği an. NoInteractableFound() NOTUNA BAK!!! 
{
	// Check Previous stuff interaction
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable) // Hala CurrentInteractable şu anda bir önceki etkileşime geçtiğimiz objede/actorde. Bunu TargetInteractable'a atıyoruz. Çünkü dataları/fonksiyonları Interface üzerinden kontrol ediyoruz. Burada Karakterin kendine ait fonksiyonlarına erişip üstündeki focus ışığını kapatıyoruz.
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable; // artık önümüzde olan bu obje/actor diyoruz
	TargetInteractable = NewInteractable; // Hedefimizdeki de bu diyoruz çünkü karşımızdaki interface sınıfını implement etmişse karşımızdakinin üzerinde olan fonksiyonu çağrıyoruz. Mesela artık bu karşımızda focus ışığını yak gibi.

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	
	TargetInteractable->BeginFocus();
}

void AInteractionSystemCharacter::NoInteractableFound() // ile sakladığımız; InteractionData.CurrentInteractable = NewInteractable; ve TargetInteractable = NewInteractable; Değişkenlerini kafamızı çevirdiğimiz an nullptr yapıyoruz!!!
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject())) // Bu process çalışmadan pickup itemi destroy edersen executable'mizi crush edebiliyor. IsNotValid'se zaten geçiyor.
		{
			TargetInteractable->EndFocus();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AInteractionSystemCharacter::BeginInteract() // E tuşuna bastığımız an çalıştırıyoruz.
{
	// verify nothing to changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			// Bu if/else yapıyı Visiual Studio Unreal Engine notlarına açıklamayla beraber koydum
			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f)) // Eğer etkileşim süresi çok kısa (0.1 saniyeden küçük) ise, doğrudan etkileşim fonksiyonu çağrılacak. Örnek Durum: Oyuncu anında bir nesne alabilir ya da bir kapı hemen açılabilir.
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&AInteractionSystemCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AInteractionSystemCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction); // Zamanlayıcı (timer), daha önce etkileşim süresi (InteractionDuration) geçene kadar çalışıyordu. Ancak etkileşim bitmiş olduğunda, bu zamanlayıcıya artık gerek kalmadığından, temizlenir.

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AInteractionSystemCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction); // Zamanlayıcı (timer), daha önce etkileşim süresi (InteractionDuration) geçene kadar çalışıyordu. Ancak etkileşim başlamış olduğunda, bu zamanlayıcıya artık gerek kalmadığından, temizlenir.

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void AInteractionSystemCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AInteractionSystemCharacter::ToggleMenu()
{
	HUD->ToggleMenu();

	if (HUD->bIsMenuVisible)
	{
		StopAiming();
	}
}

void AInteractionSystemCharacter::Aim()
{
	if (!HUD->bIsMenuVisible)
	{
		bAiming = true;
		bUseControllerRotationYaw = true; // Character rotates with the camera left and right
		GetCharacterMovement()->MaxWalkSpeed = 200.f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->PlayFromStart();
	}
}

void AInteractionSystemCharacter::StopAiming()
{
	if (bAiming)
	{
		bAiming = false;
		bUseControllerRotationYaw = false; // Character rotates with the camera left and right
		HUD->HideCrosshair();
		GetCharacterMovement()->MaxWalkSpeed = 500.f;

		if (AimingCameraTimeline)
			AimingCameraTimeline->Reverse(); // take back to the default location
	}
}

void AInteractionSystemCharacter::UpdateCameraTimeline(const float TimelineValue) const // think of this is the timeline's tick. When we have this timeline playing. Anytime this runs so, it's gonna happen on its own tick rate. Anytime this runs it's going to start this Lerp and set it
{
	const FVector CameraLocation = FMath::Lerp(DefaultCameraLocation, AimingCameraLocation, TimelineValue);
	CameraBoom->SocketOffset = CameraLocation;
}

void AInteractionSystemCharacter::CameraTimelineEnd()
{
	if (AimingCameraTimeline)
	{
		if (AimingCameraTimeline->GetPlaybackPosition() != 0.f)
		{
			HUD->ShowCrosshair();
		}
	}
}

void AInteractionSystemCharacter::DropItem(UItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop)) // so it's technically returning UItem and doing an if check to see if it's null or not
	{
		FActorSpawnParameters SpawnParams; // This is a struct containing different things you can set for spawning a new actor into the world
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // It's like you're trying to drop something right next to a wall and the point where the item would spawn would be inside the wall it will kind a move it back for you rather than failing.

		const FVector SpawnLocation{GetActorLocation() + (GetActorForwardVector() * 50.0f)};
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation); // this is like rotation, location and size all packed into one struct

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParams);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"))
	}
}


