// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPEnemySpawner.generated.h"

class AEPEnemy;

/**
 * 적 스포너. 스포너 위치를 중심으로 일정 주기마다 적을 생성한다.
 * 레벨에 배치해 SpawnAreaExtent로 스폰 범위를 조정한다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEPEnemySpawner();

	/** 스폰할 적 클래스. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AEPEnemy> EnemyClass;

	/** 스폰 주기(초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnInterval = 1.f;

	/** 스포너 위치 기준 스폰 반경(XY, 언리얼 유닛). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float SpawnAreaExtent = 900.f;

	/** 동시 생존 적 최대 수. 초과 시 스폰을 건너뛴다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 MaxAliveEnemies = 20;

protected:
	virtual void BeginPlay() override;

	/** 타이머 콜백: 적 1마리 스폰. */
	void SpawnOne();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	FTimerHandle SpawnTimerHandle;

	/** 현재 레벨의 살아있는 적 수를 센다. */
	int32 CountAliveEnemies() const;
};
