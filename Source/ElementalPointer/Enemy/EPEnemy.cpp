// Copyright Epic Games, Inc. All Rights Reserved.

#include "Enemy/EPEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

AEPEnemy::AEPEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

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
}

void AEPEnemy::ApplyDamage(float Amount)
{
	if (!IsAlive())
	{
		return;
	}

	CurrentHealth -= Amount;

	// 피격 데미지 디버그 표시.
	DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 120), FString::FromInt(FMath::RoundToInt(Amount)),
		nullptr, FColor::White, 0.5f);

	if (CurrentHealth <= 0.f)
	{
		Destroy();
	}
}
