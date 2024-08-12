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
	//(碰撞盒，角色，其他被击中的组件，3d空间垂直，击中结果）
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;

	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;


//子弹运动组件
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//添加火箭模型
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
	
	//粒子系统
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	class UParticleSystemComponent *TracerComponent;


	//撞击粒子
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	//撞击声音
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	
	bool bHitEnemy = false;

	//敌人击中粒子
	UPROPERTY()
		class UParticleSystem* EnemyParticles;
	//敌人击中声音
	UPROPERTY()
		USoundCue* EnemySound;

};
