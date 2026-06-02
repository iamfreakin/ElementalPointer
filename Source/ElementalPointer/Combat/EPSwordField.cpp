// Copyright Epic Games, Inc. All Rights Reserved.

#include "Combat/EPSwordField.h"
#include "Core/EPPlayerController.h"
#include "Enemy/EPEnemy.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
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

	// 오토어택: 공속 주기마다 범위 내 적 타격.
	if (AttacksPerSecond > 0.f)
	{
		AttackAccumulator += DeltaTime;
		const float AttackInterval = 1.f / AttacksPerSecond;
		while (AttackAccumulator >= AttackInterval)
		{
			AttackAccumulator -= AttackInterval;
			PerformAttack();
		}
	}

	if (bDrawDebug)
	{
		// 전투 평면(XY) 위에 공격 범위를 원으로 표시.
		DrawDebugCircle(GetWorld(), Cursor.WorldLocation, AttackRadius, 48, FColor::White,
			false, -1.f, 0, 3.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
	}
}

void AEPSwordField::PerformAttack()
{
	const FVector Center = GetActorLocation();

	for (TActorIterator<AEPEnemy> It(GetWorld()); It; ++It)
	{
		AEPEnemy* Enemy = *It;
		if (!Enemy || !Enemy->IsAlive())
		{
			continue;
		}

		// 전투 평면 거리(XY)로 범위 판정.
		if (FVector::Dist2D(Enemy->GetActorLocation(), Center) <= AttackRadius)
		{
			Enemy->ApplyDamage(AttackDamage);
		}
	}
}
