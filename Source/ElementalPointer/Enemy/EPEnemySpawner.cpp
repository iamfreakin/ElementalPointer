// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enemy/EPEnemySpawner.h"
#include "Enemy/EPEnemy.h"
#include "Core/EPGameMode.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AEPEnemySpawner::AEPEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// 전용 BP가 없어도 C++ 기본 클래스로 동작.
	EnemyClass = AEPEnemy::StaticClass();
}

void AEPEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEPEnemySpawner::SpawnOne, SpawnInterval, true);
	}
}

void AEPEnemySpawner::SpawnOne()
{
	if (!EnemyClass)
	{
		return;
	}

	// 전투가 끝났으면 스폰 중지.
	const AEPGameMode* GameMode = Cast<AEPGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode && !GameMode->IsCombatActive())
	{
		return;
	}

	if (MaxAliveEnemies > 0 && CountAliveEnemies() >= MaxAliveEnemies)
	{
		return;
	}

	// 스포너 위치 기준 XY 평면 랜덤 위치.
	const FVector Origin = GetActorLocation();
	const FVector SpawnLocation(
		Origin.X + FMath::RandRange(-SpawnAreaExtent, SpawnAreaExtent),
		Origin.Y + FMath::RandRange(-SpawnAreaExtent, SpawnAreaExtent),
		0.f);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AEPEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, Params);
}

int32 AEPEnemySpawner::CountAliveEnemies() const
{
	int32 Count = 0;
	for (TActorIterator<AEPEnemy> It(GetWorld()); It; ++It)
	{
		if (It->IsAlive())
		{
			++Count;
		}
	}
	return Count;
}
