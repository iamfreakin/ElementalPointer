// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPGameInstance.h"

void UEPGameInstance::EnsureNodeCount(int32 Count)
{
	if (NodeLevels.Num() < Count)
	{
		NodeLevels.SetNumZeroed(Count);
	}
}
