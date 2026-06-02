// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPGameMode.h"
#include "Core/EPPlayerController.h"

AEPGameMode::AEPGameMode()
{
	PlayerControllerClass = AEPPlayerController::StaticClass();
}
