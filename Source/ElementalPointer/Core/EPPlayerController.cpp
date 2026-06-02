// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/EPPlayerController.h"
#include "Combat/EPSwordField.h"
#include "DrawDebugHelpers.h"

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
		GetWorld()->SpawnActor<AEPSwordField>(SwordFieldClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
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
