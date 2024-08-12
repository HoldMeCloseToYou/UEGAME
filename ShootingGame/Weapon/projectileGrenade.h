// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "projectile.h"
#include "projectileGrenade.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AprojectileGrenade : public Aprojectile
{
	GENERATED_BODY()
	


public:
	AprojectileGrenade();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay()override;
	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;
};
