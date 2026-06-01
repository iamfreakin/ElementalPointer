// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EPGameMode.generated.h"

/**
 * 기본 게임 모드. 코어 전투/성장 루프의 진입점.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEPGameMode();
};
