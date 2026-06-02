// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPEnemy.generated.h"

/** 공격이 적중한 방향 유형. */
UENUM(BlueprintType)
enum class EEPHitType : uint8
{
	Normal,
	Head,	// 정면 타격 (+피해)
	Back	// 후면 타격 (치명타)
};

/**
 * 기본 적. 전투 평면 위에 존재하며, 공격 범위 안에 들어오면 오토어택 대상이 된다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPEnemy : public AActor
{
	GENERATED_BODY()

public:
	AEPEnemy();

	/** 최대 체력. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float MaxHealth = 30.f;

	/** 처치 시 지급 경험치. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Reward")
	float ExperienceReward = 10.f;

	/** 처치 시 지급 골드. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Reward")
	int32 GoldReward = 5;

	/** 피해를 적용한다. 체력이 0 이하가 되면 처치 처리. */
	void ApplyDamage(float Amount, EEPHitType HitType = EEPHitType::Normal);

	bool IsAlive() const { return CurrentHealth > 0.f; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/** 이동 속도(유닛/초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Move")
	float MoveSpeed = 120.f;

	/** 배회 경계 반경(원점 기준). 벗어나면 안쪽으로 방향 전환. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Move")
	float WanderBounds = 950.f;

	/** 적의 바라보는 방향을 디버그 화살표로 표시할지. */
	UPROPERTY(EditAnywhere, Category = "Enemy|Debug")
	bool bDrawFacingDebug = true;

private:
	/** 새 배회 방향을 무작위로 정하고 그쪽을 바라보게 한다. */
	void PickNewWanderDirection();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	float CurrentHealth = 0.f;

	/** 현재 이동 방향(= facing). */
	FVector MoveDirection = FVector::ForwardVector;

	/** 다음 방향 전환까지 남은 시간. */
	float DirChangeTimer = 0.f;
};
