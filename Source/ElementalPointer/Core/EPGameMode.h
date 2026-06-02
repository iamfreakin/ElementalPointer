// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EPGameMode.generated.h"

/** 한 판의 진행 상태. */
UENUM(BlueprintType)
enum class ECombatState : uint8
{
	InProgress,	// 전투 중 (타이머 진행)
	Shop		// 전투 종료 후 상점
};

/**
 * 기본 게임 모드. 코어 전투/성장 루프의 진입점.
 * 전투 ↔ 상점 상태를 순환하며 한 판의 흐름을 관장한다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEPGameMode();

	/** 현재 진행 상태. */
	UFUNCTION(BlueprintPure, Category = "Combat")
	ECombatState GetCombatState() const { return CombatState; }

	/** 전투가 진행 중인지(스폰 등 판정용). */
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsCombatActive() const { return CombatState == ECombatState::InProgress; }

	/** 남은 전투 시간(초). */
	UFUNCTION(BlueprintPure, Category = "Combat")
	float GetRemainingTime() const { return RemainingTime; }

	/** 새 전투를 시작한다(상점 → 다음 전투). */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombat();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** 한 판의 제한 시간(초). */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float CombatDuration = 15.f;

	/** 전투 종료 → 상점 진입. */
	void EndCombat();

private:
	float RemainingTime = 0.f;
	ECombatState CombatState = ECombatState::Shop;
};
