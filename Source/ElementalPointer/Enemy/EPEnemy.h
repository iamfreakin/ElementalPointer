// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EPEnemy.generated.h"

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

	/** 피해를 적용한다. 체력이 0 이하가 되면 처치 처리. */
	void ApplyDamage(float Amount);

	bool IsAlive() const { return CurrentHealth > 0.f; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	float CurrentHealth = 0.f;
};
