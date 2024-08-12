// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "projectile.generated.h"

UCLASS()
class SHOOTINGGAME_API Aprojectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aprojectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	//(��ײ�У���ɫ�����������е������3d�ռ䴹ֱ�����н����
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;

	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;


//�ӵ��˶����
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//��ӻ��ģ��
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;


	UPROPERTY(EditAnywhere)
	float Damage = 100.f;
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius=200.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius=500.f;
	

private:

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyTime = 3.f;
public:	
	
	//����ϵͳ
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent *TracerComponent;


	//ײ������
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	//ײ������
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	
	bool bHitEnemy = false;

	//���˻�������
	UPROPERTY()
		class UParticleSystem* EnemyParticles;
	//���˻�������
	UPROPERTY()
		USoundCue* EnemySound;

};
