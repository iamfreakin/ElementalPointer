// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EPGameInstance.generated.h"

/**
 * 게임 인스턴스. 런(전투)을 넘어 유지되는 영구 데이터(깨달음)를 보유한다.
 * 디스크 저장(SaveGame)은 2.5에서 연결 예정 — 현재는 세션 내 영속.
 */
UCLASS()
class ELEMENTALPOINTER_API UEPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/** 깨달음 포인트 잔량. */
	UPROPERTY(VisibleAnywhere, Category = "Enlightenment")
	int32 EnlightenmentPoints = 0;

	/** 깨달음 노드별 투자 레벨(인덱스 = 노드). */
	UPROPERTY(VisibleAnywhere, Category = "Enlightenment")
	TArray<int32> NodeLevels;

	/** 노드 수에 맞게 레벨 배열 크기 보장. */
	void EnsureNodeCount(int32 Count);
};
