// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPGameMode.h"
#include "Core/EPPlayerController.h"
#include "Core/EPPlayerState.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

AEPGameMode::AEPGameMode()
{
	PlayerControllerClass = AEPPlayerController::StaticClass();
	PlayerStateClass = AEPPlayerState::StaticClass();

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

		if (const APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (const AEPPlayerState* PS = PC->GetPlayerState<AEPPlayerState>())
			{
				GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Green,
					FString::Printf(TEXT("Lv.%d  XP:%.0f  Gold:%d"), PS->GetPlayerLevel(), PS->GetExperience(), PS->GetGold()));
			}
		}
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
