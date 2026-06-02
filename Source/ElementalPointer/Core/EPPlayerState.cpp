// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPPlayerState.h"

void AEPPlayerState::AddExperience(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}
	Experience += Amount;
	// TODO(2.2): 레벨업 임계 처리 + 각성 카드 트리거.
}

void AEPPlayerState::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}
	Gold += Amount;
}
