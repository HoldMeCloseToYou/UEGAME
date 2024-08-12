// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AProjectileBullet : public Aprojectile
{
	GENERATED_BODY()
	
public:
	AProjectileBullet();

	class  AWeapon* WeaponDamage;
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;

};
