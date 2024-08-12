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
	
	
	//�������
	UPROPERTY(EditAnywhere)
		class UParticleSystem* ImpactParticles;
	//������Ч
	UPROPERTY(EditAnywhere)
		USoundCue* HitSound;


private:
	
	UPROPERTY(EditAnywhere)
		class UParticleSystem* BeamParticles;

	//ǹ������
	UPROPERTY(EditAnywhere)
		class UParticleSystem* MuzzleFlash;

	//��������
	UPROPERTY(EditAnywhere)
		USoundCue* FireSound;
	

	UPROPERTY()
		class UNiagaraComponent* BeamSystemComponent;

	//��������ľ���
	UPROPERTY(EditAnywhere,Category="Weaopon Scatter")
	float DistanceToSphere = 800.f;

	//����뾶
	UPROPERTY(EditAnywhere,Category="Weapon Scatter")
		float SphereRadius = 75.f;

	//�Ƿ�ʹ��ɢ��
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

};
