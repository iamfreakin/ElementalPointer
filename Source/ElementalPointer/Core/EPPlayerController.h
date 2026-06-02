// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Progression/EPAwakening.h"
#include "EPPlayerController.generated.h"

class AEPSwordField;

/** 상점 항목. 구매할 때마다 가격이 오른다(반복 구매). */
USTRUCT()
struct FShopItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	EAwakeningStat Stat = EAwakeningStat::AttackDamage;

	UPROPERTY()
	float Value = 0.f;

	UPROPERTY()
	int32 BaseCost = 10;

	UPROPERTY()
	int32 CostGrowth = 5;

	UPROPERTY()
	int32 PurchaseCount = 0;

	int32 CurrentCost() const { return BaseCost + PurchaseCount * CostGrowth; }
};

/** 깨달음 노드. 주효과 + 페널티(트레이드오프)를 레벨당 적용. 환불 무료. */
USTRUCT()
struct FEnlightenmentNode
{
	GENERATED_BODY()

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	EAwakeningStat MainStat = EAwakeningStat::AttackDamage;

	UPROPERTY()
	float MainValue = 0.f;

	UPROPERTY()
	EAwakeningStat PenaltyStat = EAwakeningStat::AttackSpeed;

	UPROPERTY()
	float PenaltyValue = 0.f;
};

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
	virtual void SetupInputComponent() override;

	/** BeginPlay에서 스폰할 검의 공격 범위 액터 클래스. */
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AEPSwordField> SwordFieldClass;

	// --- 각성 카드 (2.2) ---

	/** 레벨업 콜백: 카드 선택 시퀀스 진입. */
	void HandleLevelUp();

	/** 카드 풀에서 3장을 뽑아 화면에 제시(일시정지). */
	void PresentCards();

	/** index번 카드를 선택하여 적용하고 다음 큐 처리. */
	void SelectCard(int32 Index);

	void OnSelectCard1();
	void OnSelectCard2();
	void OnSelectCard3();

	/** 선택한 카드 효과를 SwordField에 적용. */
	void ApplyCard(const FAwakeningCard& Card);

	/** 기본 카드 풀 구성(임시 하드코딩, 2.3에서 DataAsset 이전). */
	void BuildDefaultCardPool();

	/** 스탯을 SwordField에 적용(각성·상점 공통). */
	void ApplyStatToSword(EAwakeningStat Stat, float Value);

	// --- 업그레이드 상점 (2.1) ---

	/** 기본 상점 항목 구성. */
	void BuildShopItems();

	/** index번 상점 항목 구매 시도(골드 차감 + 적용). */
	void BuyShopItem(int32 Index);

	/** 상점 → 다음 전투 시작. */
	void StartNextCombat();

	void OnNextCombat();

	// --- 깨달음 (2.4, 영구) ---

	/** 깨달음 노드 구성. */
	void BuildEnlightenmentNodes();

	/** GameInstance에 저장된 깨달음 레벨을 SwordField에 적용(게임 시작 시). */
	void ApplyEnlightenmentToSword();

	/** index번 깨달음 노드에 1포인트 투자. */
	void InvestEnlightenment(int32 Index);

	/** 모든 깨달음 노드 환불(포인트 회수, 효과 제거). */
	void RefundAllEnlightenment();

	void OnInvest1();
	void OnInvest2();
	void OnInvest3();
	void OnInvest4();
	void OnRefundEnlightenment();

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

	/** 스폰한 공격 범위 액터(카드 효과 적용 대상). */
	TObjectPtr<AEPSwordField> SwordField;

	/** 각성 카드 풀. */
	TArray<FAwakeningCard> CardPool;

	/** 현재 제시 중인 3장. */
	TArray<FAwakeningCard> CurrentChoices;

	/** 처리 대기 중인 레벨업 수. */
	int32 PendingLevelUps = 0;

	/** 카드 선택 대기 상태. */
	bool bAwaitingSelection = false;

	/** 상점 항목. */
	TArray<FShopItem> ShopItems;

	/** 깨달음 노드 정의. */
	TArray<FEnlightenmentNode> EnlightenmentNodes;
};
