// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/EPSwordField.h"
#include "Core/EPPlayerController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AEPSwordField::AEPSwordField()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AEPSwordField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const AEPPlayerController* PC = Cast<AEPPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC)
	{
		return;
	}

	const FCursorState& Cursor = PC->GetCursorState();
	if (!Cursor.bValid)
	{
		return;
	}

	SetActorLocation(Cursor.WorldLocation);

	if (bDrawDebug)
	{
		// 전투 평면(XY) 위에 공격 범위를 원으로 표시.
		DrawDebugCircle(GetWorld(), Cursor.WorldLocation, AttackRadius, 48, FColor::White,
			false, -1.f, 0, 3.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	}
}
