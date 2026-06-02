// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPPlayerController.h"
#include "Core/EPPlayerState.h"
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

void AEPPlayerController::OnSelectCard1() { SelectCard(0); }
void AEPPlayerController::OnSelectCard2() { SelectCard(1); }
void AEPPlayerController::OnSelectCard3() { SelectCard(2); }

void AEPPlayerController::ApplyCard(const FAwakeningCard& Card)
{
	if (!SwordField)
	{
		return;
	}

	switch (Card.Stat)
	{
	case EAwakeningStat::AttackDamage:    SwordField->AttackDamage += Card.Value; break;
	case EAwakeningStat::AttackSpeed:     SwordField->AttacksPerSecond += Card.Value; break;
	case EAwakeningStat::AttackRadius:    SwordField->AttackRadius += Card.Value; break;
	case EAwakeningStat::HeadMultiplier:  SwordField->HeadAttackMultiplier += Card.Value; break;
	case EAwakeningStat::BackMultiplier:  SwordField->BackAttackCritMultiplier += Card.Value; break;
	}
}

void AEPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateCursorState();

	if (bDrawCursorDebug && CursorState.bValid)
	{
		DrawDebugSphere(GetWorld(), CursorState.WorldLocation, 40.f, 16, FColor::Cyan);
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
