// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionSystemGameMode.h"
#include "InteractionSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInteractionSystemGameMode::AInteractionSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
