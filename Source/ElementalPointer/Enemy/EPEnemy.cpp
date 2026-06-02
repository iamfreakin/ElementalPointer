// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enemy/EPEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

AEPEnemy::AEPEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// 아트 미생성 단계: 엔진 기본 구체로 임시 표현.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.8f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void AEPEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	PickNewWanderDirection();
}

void AEPEnemy::PickNewWanderDirection()
{
	const float Yaw = FMath::FRandRange(0.f, 360.f);
	MoveDirection = FRotator(0.f, Yaw, 0.f).Vector();
	DirChangeTimer = FMath::FRandRange(1.5f, 3.f);
	SetActorRotation(MoveDirection.Rotation());
}

void AEPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 배회 이동.
	DirChangeTimer -= DeltaTime;
	if (DirChangeTimer <= 0.f)
	{
		PickNewWanderDirection();
	}

	FVector NewLocation = GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime;
	NewLocation.Z = 0.f;

	// 배회 경계를 벗어나면 중심 방향으로 전환.
	if (FVector::Dist2D(NewLocation, FVector::ZeroVector) > WanderBounds)
	{
		MoveDirection = (-NewLocation).GetSafeNormal2D();
		SetActorRotation(MoveDirection.Rotation());
		DirChangeTimer = FMath::FRandRange(1.5f, 3.f);
	}

	SetActorLocation(NewLocation);

	if (bDrawFacingDebug)
	{
		const FVector Start = GetActorLocation();
		DrawDebugDirectionalArrow(GetWorld(), Start, Start + GetActorForwardVector() * 120.f,
			60.f, FColor::Green, false, -1.f, 0, 4.f);
	}
}

void AEPEnemy::ApplyDamage(float Amount, EEPHitType HitType)
{
	if (!IsAlive())
	{
		return;
	}

	CurrentHealth -= Amount;

	// 히트 타입별 색: 일반=흰색, 헤드=노랑, 백=주황 (아트 정의와 일치).
	FColor DamageColor = FColor::White;
	switch (HitType)
	{
	case EEPHitType::Head: DamageColor = FColor::Yellow; break;
	case EEPHitType::Back: DamageColor = FColor::Orange; break;
	default: break;
	}

	DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 120), FString::FromInt(FMath::RoundToInt(Amount)),
		nullptr, DamageColor, 0.5f);

	if (CurrentHealth <= 0.f)
	{
		Destroy();
	}
}
