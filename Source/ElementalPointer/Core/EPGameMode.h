// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EPGameMode.generated.h"

/**
 * 기본 게임 모드. 코어 전투/성장 루프의 진입점.
 * 한 판의 전투 타이머와 진행 상태를 관장한다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEPGameMode();

	/** 전투가 진행 중인지. */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsCombatActive() const { return bCombatActive; }

	/** 남은 전투 시간(초). */
	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetRemainingTime() const { return RemainingTime; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** 한 판의 제한 시간(초). */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CombatDuration = 15.f;

	/** 전투 종료 처리. */
	void EndCombat();

private:
	float RemainingTime = 0.f;
	bool bCombatActive = false;
};
