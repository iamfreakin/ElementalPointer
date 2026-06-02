// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EPAwakening.generated.h"

/** 각성 카드가 수정하는 스탯 종류. (스탯형 카드 — 메커니즘형은 추후) */
UENUM(BlueprintType)
enum class EAwakeningStat : uint8
{
	AttackDamage,
	AttackSpeed,
	AttackRadius,
	HeadMultiplier,
	BackMultiplier
};

/** 각성 카드 한 장. 선택 시 해당 스탯을 Value만큼 증가시킨다. */
USTRUCT(BlueprintType)
struct FAwakeningCard
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAwakeningStat Stat = EAwakeningStat::AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;

	FAwakeningCard() {}
	FAwakeningCard(const FString& InName, EAwakeningStat InStat, float InValue)
		: DisplayName(InName), Stat(InStat), Value(InValue) {}
};
