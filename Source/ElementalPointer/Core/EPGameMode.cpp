// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPGameMode.h"
#include "Core/EPPlayerController.h"
#include "Engine/Engine.h"

AEPGameMode::AEPGameMode()
{
	PlayerControllerClass = AEPPlayerController::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void AEPGameMode::BeginPlay()
{
	Super::BeginPlay();

	RemainingTime = CombatDuration;
	bCombatActive = true;
}

void AEPGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bCombatActive)
	{
		return;
	}

	RemainingTime -= DeltaSeconds;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow,
			FString::Printf(TEXT("전투 남은 시간: %.1fs"), FMath::Max(RemainingTime, 0.f)));
	}

	if (RemainingTime <= 0.f)
	{
		EndCombat();
	}
}

void AEPGameMode::EndCombat()
{
	bCombatActive = false;
	RemainingTime = 0.f;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, TEXT("전투 종료"));
	}
}
