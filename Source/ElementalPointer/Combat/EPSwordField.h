// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPSwordField.generated.h"

/**
 * 검의 공격 범위. 플레이어 = 커서 = 이 원형 범위.
 * 매 틱 커서의 전투 평면 투영 위치로 이동하며, 범위 안의 적이 오토어택 대상이 된다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPSwordField : public AActor
{
	GENERATED_BODY()

public:
	AEPSwordField();

	/** 공격 범위 반경(언리얼 유닛). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRadius = 250.f;

	float GetAttackRadius() const { return AttackRadius; }

	/** 한 번의 오토어택 피해량. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 10.f;

	/** 초당 공격 횟수(공속). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttacksPerSecond = 1.f;

	/** 헤드어택(정면) 피해 배수. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HeadAttackMultiplier = 1.2f;

	/** 백어택(후면) 치명타 피해 배수. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BackAttackCritMultiplier = 2.f;

protected:
	virtual void Tick(float DeltaTime) override;

	/** 범위 안의 모든 살아있는 적에게 1회 오토어택을 적용. */
	void PerformAttack();

	/** 범위를 디버그 원으로 표시할지. */
	UPROPERTY(EditAnywhere, Category = "Combat|Debug")
	bool bDrawDebug = true;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	/** 공격 주기 누적 시간. */
	float AttackAccumulator = 0.f;
};
