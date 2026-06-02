// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "EPPlayerState.generated.h"

/**
 * 한 판(런)의 성장 상태. 경험치/레벨(각성용)과 골드(상점용)를 보유한다.
 * 깨달음(영구 성장)은 별도 세이브로 관리 예정.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetPlayerLevel() const { return Level; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	float GetExperience() const { return Experience; }

	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetGold() const { return Gold; }

	/** 경험치 획득. (레벨업 처리는 2.2에서 추가) */
	void AddExperience(float Amount);

	/** 골드 획득. */
	void AddGold(int32 Amount);

private:
	UPROPERTY(VisibleAnywhere, Category = "Progression")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, Category = "Progression")
	float Experience = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Progression")
	int32 Gold = 0;
};
