// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPPlayerState.h"

void AEPPlayerState::AddExperience(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}
	Experience += Amount;

	// 임계 도달 시마다 레벨업 (한 번에 여러 레벨도 처리).
	while (Experience >= GetExperienceToNextLevel())
	{
		Experience -= GetExperienceToNextLevel();
		++Level;
		OnLevelUp.Broadcast();
	}
}

void AEPPlayerState::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}
	Gold += Amount;
}

bool AEPPlayerState::TrySpendGold(int32 Amount)
{
	if (Amount <= 0 || Gold < Amount)
	{
		return false;
	}
	Gold -= Amount;
	return true;
}
