// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const FVector& HitTarget)override;


protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart,const FVector& HitTarget,FHitResult& OutHit);
	
	
	//冲击粒子
	UPROPERTY(EditAnywhere)
		class UParticleSystem* ImpactParticles;
	//击中音效
	UPROPERTY(EditAnywhere)
		USoundCue* HitSound;


private:
	
	UPROPERTY(EditAnywhere)
		class UParticleSystem* BeamParticles;

	//枪口闪光
	UPROPERTY(EditAnywhere)
		class UParticleSystem* MuzzleFlash;

	//开火声音
	UPROPERTY(EditAnywhere)
		USoundCue* FireSound;
	

	UPROPERTY()
		class UNiagaraComponent* BeamSystemComponent;

	//到达球体的距离
	UPROPERTY(EditAnywhere,Category="Weaopon Scatter")
	float DistanceToSphere = 800.f;

	//球体半径
	UPROPERTY(EditAnywhere,Category="Weapon Scatter")
		float SphereRadius = 75.f;

	//是否使用散射
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

};
