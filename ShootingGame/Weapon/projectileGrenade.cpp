// Fill out your copyright notice in the Description page of Project Settings.


#include "projectileGrenade.h"
#include"GameFramework/ProjectileMovementComponent.h"
#include"Kismet/GameplayStatics.h"

AprojectileGrenade::AprojectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	//是否允许反弹
	ProjectileMovementComponent->bShouldBounce = true;
}

void AprojectileGrenade::BeginPlay()
{
	AActor::BeginPlay();
	SpawnTrailSystem();
	StartDestroyTimer();

	//当反弹的时候调用的函数
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AprojectileGrenade::OnBounce);
}

void AprojectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}

void AprojectileGrenade::Destroyed()
{
	ExplodeDamage();
	Super::Destroyed();
}