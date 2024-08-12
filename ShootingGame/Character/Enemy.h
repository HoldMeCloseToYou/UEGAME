// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTINGGAME_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	class UCombatComponent* Combat;

	//击中敌人粒子
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=Combat)
		class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		class USoundCue* ImpactSound;

	//实时血量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float Health;

	//最大血量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float MaxHealth;

	//获取头部
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FString HeadBone;


	//显示健康
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float HealthBarDisplayTime;


	FTimerHandle HealthBarTimer;

	//击中动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UAnimMontage* HitMontage;


	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float HitReactTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float HitReactTimeMax;

	bool bCanHitReact;

	UPROPERTY(VisibleAnywhere,Category=Combat)
	TMap<UUserWidget*, FVector>HitNumbers;

	UPROPERTY(EditAnywhere, Category = Combat) 
	float HitNumberDestroyTime;


	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation,bool bHeadShot);



	//AI行为树
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
	class UBehaviorTree* BehaviorTree;

	//巡逻点
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Behavior Tree",meta=(MakeEditWidget="true"))
	FVector PatrolPoint;

	//巡逻点2
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Behavior Tree", meta = (MakeEditWidget = "true"))
		FVector PatrolPoint2;

	class AEnemyAIController* EnemyController;

	//敌人进行攻击的区域
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Combat)
	class USphereComponent* AgroSphere;


	//击晕
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool bStunned;

	//被击晕的几率
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float StunChance;

	//在范围内攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool bInAttackRange;


	//攻击领域范围
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	USphereComponent* CombatRangeSphere;


	//敌人攻击动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		UAnimMontage* AttackMontage;

	//四个不同的攻击动作
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;


	//左手武器碰撞体积
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Combat)
	class UBoxComponent* LeftWeaponCollision;
	
	//右手武器碰撞体积
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Combat)
	class UBoxComponent* RightWeaponCollision;


	//敌人攻击伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float BaseDamage;


	//敌人攻击伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FName LeftWeaponSocket;


	//敌人攻击伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FName RightWeaponSocket;


	//可以攻击
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	//攻击最短等待时间
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitTime;

	//敌人死亡动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	UAnimMontage* DeathMontage;
	
	bool bDying;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float DeathTime;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HidHealthBar();
	

	void Die();

	void PlayHitMontage(FName Section, float PlayRate = 1.0f);

	void ResetHitReactTimer();

	//使用蓝图存储显示在屏幕的伤害数值
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	//将显示在屏幕的伤害数值
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	//敌人进行攻击的区域判断,使用UFUNCTION()
	UFUNCTION()
	void AgroSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent*OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);


	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION()
	void CombatRangeOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void CombatRangeEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	//播放敌人攻击动画
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate = 1.0f);



	//随机选择攻击方式
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	//左手武器重叠
		UFUNCTION()
	void OnLeftWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);


	//右手武器重叠
		UFUNCTION()
		void OnRightWeaponOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
			);

		//开启和停用左手武器
		UFUNCTION(BlueprintCallable)
		void ActivateLeftWeapon();
		UFUNCTION(BlueprintCallable)
		void DeactivateLeftWeapon();

		//开启和停用左手武器
		UFUNCTION(BlueprintCallable)
		void ActivateRightWeapon();
		UFUNCTION(BlueprintCallable)
		void DeactivateRightWeapon();

		//造成伤害的函数
		void DoDamage(class ADCharacter* Victim);

		//造成流血
		void SpawnBlood(ADCharacter* Victim,FName SocketName);


		//击晕角色
		void StunCharacter(ADCharacter* Victim);

		void ResetCanAttack();

		UFUNCTION(BlueprintCallable)
		void FinishDeath();

		UFUNCTION()
		void DestroyEnemy();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterContorller)override;


	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)override;

	FORCEINLINE FString GetHeadBone()const{ return HeadBone; }


	FORCEINLINE UBehaviorTree* GetBehaviorTree()const{ return BehaviorTree; }
};
