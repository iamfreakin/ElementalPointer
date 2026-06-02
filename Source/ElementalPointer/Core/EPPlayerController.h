// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EPPlayerController.generated.h"

/**
 * 커서 상태 스냅샷. 전투/기믹 시스템은 이 구조체만 참조한다.
 */
USTRUCT(BlueprintType)
struct FCursorState
{
	GENERATED_BODY()

	/** 커서가 전투 평면(Z=0)에 투영된 월드 위치. */
	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation = FVector::ZeroVector;

	/** 이번 프레임 커서가 평면 위 유효 지점을 가리키는지. */
	UPROPERTY(BlueprintReadOnly)
	bool bValid = false;
};

/**
 * 커서 기반 전투의 입력 진입점.
 * 매 틱 화면 커서를 전투 평면(Z=0)에 투영하여 FCursorState로 노출한다.
 */
UCLASS()
class ELEMENTALPOINTER_API AEPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEPPlayerController();

	/** 현재 커서 상태(전투 평면 투영 결과). */
	UFUNCTION(BlueprintPure, Category = "Cursor")
	const FCursorState& GetCursorState() const { return CursorState; }

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

	/** BeginPlay에서 스폰할 검의 공격 범위 액터 클래스. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AEPSwordField> SwordFieldClass;

	/** 전투 평면의 높이(Z). arena 바닥과 일치. */
	UPROPERTY(EditDefaultsOnly, Category = "Cursor")
	float CombatPlaneZ = 0.f;

	/** 커서 투영 위치를 디버그 구체로 표시할지. */
	UPROPERTY(EditAnywhere, Category = "Cursor|Debug")
	bool bDrawCursorDebug = true;

private:
	/** 화면 커서를 전투 평면에 투영하여 CursorState를 갱신한다. */
	void UpdateCursorState();

	FCursorState CursorState;
};
