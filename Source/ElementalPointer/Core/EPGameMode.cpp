// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPGameMode.h"
#include "Core/EPPlayerController.h"
#include "Core/EPPlayerState.h"
#include "Core/EPGameInstance.h"
#include "Enemy/EPEnemy.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AEPGameMode::AEPGameMode()
{
	PlayerControllerClass = AEPPlayerController::StaticClass();
	PlayerStateClass = AEPPlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void AEPGameMode::BeginPlay()
{
	Super::BeginPlay();

	StartCombat();
}

void AEPGameMode::StartCombat()
{
	RemainingTime = CombatDuration;
	CombatState = ECombatState::InProgress;
}

void AEPGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CombatState != ECombatState::InProgress)
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
	CombatState = ECombatState::Shop;
	RemainingTime = 0.f;

	// 남은 적 정리.
	for (TActorIterator<AEPEnemy> It(GetWorld()); It; ++It)
	{
		It->Destroy();
	}

	// 깨달음 포인트 지급 (임시: 보스 처치 보상은 페이즈 4에서 대체).
	if (UEPGameInstance* GI = GetGameInstance<UEPGameInstance>())
	{
		GI->EnlightenmentPoints += 1;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Red, TEXT("전투 종료 — 상점"));
	}
}
