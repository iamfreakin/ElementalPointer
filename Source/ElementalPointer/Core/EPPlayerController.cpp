// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPPlayerController.h"
#include "Core/EPPlayerState.h"
#include "Core/EPGameMode.h"
#include "Core/EPGameInstance.h"
#include "Combat/EPSwordField.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"

AEPPlayerController::AEPPlayerController()
{
	bShowMouseCursor = true;

	// 기본값: 전용 BP가 없어도 C++ 기본 클래스로 동작.
	SwordFieldClass = AEPSwordField::StaticClass();
}

void AEPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 커서가 화면 안에서만 움직이는 전투용 입력 모드.
	SetInputMode(FInputModeGameAndUI());

	// 검의 공격 범위 액터 스폰 (커서를 따라다님).
	if (SwordFieldClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SwordField = GetWorld()->SpawnActor<AEPSwordField>(SwordFieldClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	}

	BuildDefaultCardPool();
	BuildShopItems();
	BuildEnlightenmentNodes();

	// 영구 깨달음 효과를 시작 시 적용.
	ApplyEnlightenmentToSword();

	// 레벨업 → 각성 카드 시퀀스 구독.
	if (AEPPlayerState* PS = GetPlayerState<AEPPlayerState>())
	{
		PS->OnLevelUp.AddUObject(this, &AEPPlayerController::HandleLevelUp);
	}
}

void AEPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 각성 카드 선택용 숫자키 (임시 UI).
	InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AEPPlayerController::OnSelectCard1);
	InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AEPPlayerController::OnSelectCard2);
	InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AEPPlayerController::OnSelectCard3);

	// 상점 → 다음 전투.
	InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AEPPlayerController::OnNextCombat);

	// 깨달음 투자(4/5/6/7) + 전체 환불(R).
	InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AEPPlayerController::OnInvest1);
	InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AEPPlayerController::OnInvest2);
	InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &AEPPlayerController::OnInvest3);
	InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &AEPPlayerController::OnInvest4);
	InputComponent->BindKey(EKeys::R, IE_Pressed, this, &AEPPlayerController::OnRefundEnlightenment);
}

void AEPPlayerController::BuildDefaultCardPool()
{
	CardPool = {
		FAwakeningCard(TEXT("공격력 +5"),    EAwakeningStat::AttackDamage,   5.f),
		FAwakeningCard(TEXT("공격력 +10"),   EAwakeningStat::AttackDamage,   10.f),
		FAwakeningCard(TEXT("공속 +0.3"),    EAwakeningStat::AttackSpeed,    0.3f),
		FAwakeningCard(TEXT("범위 +50"),     EAwakeningStat::AttackRadius,   50.f),
		FAwakeningCard(TEXT("헤드 배수 +0.2"), EAwakeningStat::HeadMultiplier, 0.2f),
		FAwakeningCard(TEXT("백 배수 +0.3"),  EAwakeningStat::BackMultiplier, 0.3f),
	};
}

void AEPPlayerController::HandleLevelUp()
{
	++PendingLevelUps;
	if (!bAwaitingSelection)
	{
		PresentCards();
	}
}

void AEPPlayerController::PresentCards()
{
	// 대기 큐 소진 → 종료 및 재개.
	if (PendingLevelUps <= 0 || CardPool.Num() == 0)
	{
		bAwaitingSelection = false;
		// 게임 시간 재개.
		UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
		if (GEngine)
		{
			GEngine->RemoveOnScreenDebugMessage(20);
			GEngine->RemoveOnScreenDebugMessage(21);
			GEngine->RemoveOnScreenDebugMessage(22);
			GEngine->RemoveOnScreenDebugMessage(23);
		}
		return;
	}

	bAwaitingSelection = true;
	// 게임 시간 정지 (입력은 실시간이라 숫자키 선택 가능).
	UGameplayStatics::SetGlobalTimeDilation(this, 0.f);

	// 풀에서 중복 없이 3장(또는 풀 크기만큼) 무작위 추출.
	TArray<FAwakeningCard> Pool = CardPool;
	CurrentChoices.Empty();
	const int32 Num = FMath::Min(3, Pool.Num());
	for (int32 i = 0; i < Num; ++i)
	{
		const int32 Idx = FMath::RandRange(0, Pool.Num() - 1);
		CurrentChoices.Add(Pool[Idx]);
		Pool.RemoveAt(Idx);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(20, 9999.f, FColor::Cyan, TEXT("=== 레벨업! 각성 카드 선택 (1/2/3) ==="));
		for (int32 i = 0; i < CurrentChoices.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(21 + i, 9999.f, FColor::White,
				FString::Printf(TEXT("%d: %s"), i + 1, *CurrentChoices[i].DisplayName));
		}
	}
}

void AEPPlayerController::SelectCard(int32 Index)
{
	if (!bAwaitingSelection || !CurrentChoices.IsValidIndex(Index))
	{
		return;
	}

	ApplyCard(CurrentChoices[Index]);
	--PendingLevelUps;

	// 다음 큐 처리(없으면 재개).
	PresentCards();
}

void AEPPlayerController::OnSelectCard1() { bAwaitingSelection ? SelectCard(0) : BuyShopItem(0); }
void AEPPlayerController::OnSelectCard2() { bAwaitingSelection ? SelectCard(1) : BuyShopItem(1); }
void AEPPlayerController::OnSelectCard3() { bAwaitingSelection ? SelectCard(2) : BuyShopItem(2); }

void AEPPlayerController::ApplyCard(const FAwakeningCard& Card)
{
	ApplyStatToSword(Card.Stat, Card.Value);
}

void AEPPlayerController::ApplyStatToSword(EAwakeningStat Stat, float Value)
{
	if (!SwordField)
	{
		return;
	}

	switch (Stat)
	{
	case EAwakeningStat::AttackDamage:    SwordField->AttackDamage += Value; break;
	case EAwakeningStat::AttackSpeed:     SwordField->AttacksPerSecond += Value; break;
	case EAwakeningStat::AttackRadius:    SwordField->AttackRadius += Value; break;
	case EAwakeningStat::HeadMultiplier:  SwordField->HeadAttackMultiplier += Value; break;
	case EAwakeningStat::BackMultiplier:  SwordField->BackAttackCritMultiplier += Value; break;
	}
}

void AEPPlayerController::BuildShopItems()
{
	ShopItems.Empty();
	FShopItem Dmg;   Dmg.DisplayName = TEXT("공격력 +2");  Dmg.Stat = EAwakeningStat::AttackDamage; Dmg.Value = 2.f;  Dmg.BaseCost = 10; Dmg.CostGrowth = 5;
	FShopItem Spd;   Spd.DisplayName = TEXT("공속 +0.1");  Spd.Stat = EAwakeningStat::AttackSpeed;  Spd.Value = 0.1f; Spd.BaseCost = 15; Spd.CostGrowth = 8;
	FShopItem Rad;   Rad.DisplayName = TEXT("범위 +20");   Rad.Stat = EAwakeningStat::AttackRadius; Rad.Value = 20.f; Rad.BaseCost = 12; Rad.CostGrowth = 6;
	ShopItems.Add(Dmg);
	ShopItems.Add(Spd);
	ShopItems.Add(Rad);
}

void AEPPlayerController::BuyShopItem(int32 Index)
{
	if (!ShopItems.IsValidIndex(Index))
	{
		return;
	}

	// 상점 상태에서만 구매 가능.
	const AEPGameMode* GM = GetWorld()->GetAuthGameMode<AEPGameMode>();
	if (!GM || GM->GetCombatState() != ECombatState::Shop)
	{
		return;
	}

	AEPPlayerState* PS = GetPlayerState<AEPPlayerState>();
	if (!PS)
	{
		return;
	}

	FShopItem& Item = ShopItems[Index];
	if (PS->TrySpendGold(Item.CurrentCost()))
	{
		ApplyStatToSword(Item.Stat, Item.Value);
		++Item.PurchaseCount;
	}
}

void AEPPlayerController::StartNextCombat()
{
	if (AEPGameMode* GM = GetWorld()->GetAuthGameMode<AEPGameMode>())
	{
		GM->StartCombat();
	}
}

void AEPPlayerController::OnNextCombat()
{
	if (AEPGameMode* GM = GetWorld()->GetAuthGameMode<AEPGameMode>())
	{
		if (GM->GetCombatState() == ECombatState::Shop)
		{
			StartNextCombat();
		}
	}
}

void AEPPlayerController::BuildEnlightenmentNodes()
{
	EnlightenmentNodes.Empty();
	auto MakeNode = [](const FString& Name, EAwakeningStat Main, float MainVal, EAwakeningStat Pen, float PenVal)
	{
		FEnlightenmentNode N;
		N.DisplayName = Name; N.MainStat = Main; N.MainValue = MainVal; N.PenaltyStat = Pen; N.PenaltyValue = PenVal;
		return N;
	};
	// 트레이드오프 노드 4종.
	EnlightenmentNodes.Add(MakeNode(TEXT("공격(공격+5/공속-0.1)"),  EAwakeningStat::AttackDamage,  5.f,  EAwakeningStat::AttackSpeed,  -0.1f));
	EnlightenmentNodes.Add(MakeNode(TEXT("속도(공속+0.2/범위-15)"), EAwakeningStat::AttackSpeed,   0.2f, EAwakeningStat::AttackRadius, -15.f));
	EnlightenmentNodes.Add(MakeNode(TEXT("범위(범위+40/공속-0.1)"), EAwakeningStat::AttackRadius,  40.f, EAwakeningStat::AttackSpeed,  -0.1f));
	EnlightenmentNodes.Add(MakeNode(TEXT("치명(백배수+0.3/공격-3)"), EAwakeningStat::BackMultiplier, 0.3f, EAwakeningStat::AttackDamage, -3.f));
}

void AEPPlayerController::ApplyEnlightenmentToSword()
{
	UEPGameInstance* GI = GetGameInstance<UEPGameInstance>();
	if (!GI)
	{
		return;
	}
	GI->EnsureNodeCount(EnlightenmentNodes.Num());

	for (int32 i = 0; i < EnlightenmentNodes.Num(); ++i)
	{
		const int32 Level = GI->NodeLevels[i];
		if (Level > 0)
		{
			ApplyStatToSword(EnlightenmentNodes[i].MainStat, EnlightenmentNodes[i].MainValue * Level);
			ApplyStatToSword(EnlightenmentNodes[i].PenaltyStat, EnlightenmentNodes[i].PenaltyValue * Level);
		}
	}
}

void AEPPlayerController::InvestEnlightenment(int32 Index)
{
	if (!EnlightenmentNodes.IsValidIndex(Index))
	{
		return;
	}
	UEPGameInstance* GI = GetGameInstance<UEPGameInstance>();
	if (!GI || GI->EnlightenmentPoints <= 0)
	{
		return;
	}
	GI->EnsureNodeCount(EnlightenmentNodes.Num());

	GI->EnlightenmentPoints -= 1;
	GI->NodeLevels[Index] += 1;

	// 1레벨치 효과 즉시 적용.
	ApplyStatToSword(EnlightenmentNodes[Index].MainStat, EnlightenmentNodes[Index].MainValue);
	ApplyStatToSword(EnlightenmentNodes[Index].PenaltyStat, EnlightenmentNodes[Index].PenaltyValue);
}

void AEPPlayerController::RefundAllEnlightenment()
{
	UEPGameInstance* GI = GetGameInstance<UEPGameInstance>();
	if (!GI)
	{
		return;
	}
	GI->EnsureNodeCount(EnlightenmentNodes.Num());

	for (int32 i = 0; i < EnlightenmentNodes.Num(); ++i)
	{
		const int32 Level = GI->NodeLevels[i];
		if (Level > 0)
		{
			// 효과 역적용 + 포인트 회수.
			ApplyStatToSword(EnlightenmentNodes[i].MainStat, -EnlightenmentNodes[i].MainValue * Level);
			ApplyStatToSword(EnlightenmentNodes[i].PenaltyStat, -EnlightenmentNodes[i].PenaltyValue * Level);
			GI->EnlightenmentPoints += Level;
			GI->NodeLevels[i] = 0;
		}
	}
}

void AEPPlayerController::OnInvest1() { InvestEnlightenment(0); }
void AEPPlayerController::OnInvest2() { InvestEnlightenment(1); }
void AEPPlayerController::OnInvest3() { InvestEnlightenment(2); }
void AEPPlayerController::OnInvest4() { InvestEnlightenment(3); }
void AEPPlayerController::OnRefundEnlightenment() { RefundAllEnlightenment(); }

void AEPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateCursorState();

	if (bDrawCursorDebug && CursorState.bValid)
	{
		DrawDebugSphere(GetWorld(), CursorState.WorldLocation, 40.f, 16, FColor::Cyan);
	}

	// 상점 상태이면 항목/가격을 화면에 표시.
	const AEPGameMode* GM = GetWorld()->GetAuthGameMode<AEPGameMode>();
	if (GM && GM->GetCombatState() == ECombatState::Shop && GEngine)
	{
		const int32 CurrentGold = GetPlayerState<AEPPlayerState>() ? GetPlayerState<AEPPlayerState>()->GetGold() : 0;
		GEngine->AddOnScreenDebugMessage(30, 0.f, FColor::Cyan,
			FString::Printf(TEXT("=== 상점 ===  Gold:%d  (Space: 다음 전투)"), CurrentGold));
		for (int32 i = 0; i < ShopItems.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(31 + i, 0.f, FColor::White,
				FString::Printf(TEXT("%d: %s  (가격 %d)"), i + 1, *ShopItems[i].DisplayName, ShopItems[i].CurrentCost()));
		}

		// 깨달음(영구) 표시.
		if (const UEPGameInstance* GI = GetGameInstance<UEPGameInstance>())
		{
			GEngine->AddOnScreenDebugMessage(40, 0.f, FColor::Yellow,
				FString::Printf(TEXT("--- 깨달음 (포인트:%d)  [R: 전체 환불] ---"), GI->EnlightenmentPoints));
			for (int32 i = 0; i < EnlightenmentNodes.Num(); ++i)
			{
				const int32 Lv = GI->NodeLevels.IsValidIndex(i) ? GI->NodeLevels[i] : 0;
				GEngine->AddOnScreenDebugMessage(41 + i, 0.f, FColor::Yellow,
					FString::Printf(TEXT("%d: %s  (Lv.%d)"), 4 + i, *EnlightenmentNodes[i].DisplayName, Lv));
			}
		}
	}
}

void AEPPlayerController::UpdateCursorState()
{
	FVector WorldOrigin;
	FVector WorldDirection;
	if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		CursorState.bValid = false;
		return;
	}

	// 평면(Z=CombatPlaneZ)과 ray의 교차. 평면을 향하지 않으면 무효.
	if (FMath::IsNearlyZero(WorldDirection.Z))
	{
		CursorState.bValid = false;
		return;
	}

	const float T = (CombatPlaneZ - WorldOrigin.Z) / WorldDirection.Z;
	if (T <= 0.f)
	{
		CursorState.bValid = false;
		return;
	}

	CursorState.WorldLocation = WorldOrigin + WorldDirection * T;
	CursorState.bValid = true;
}
