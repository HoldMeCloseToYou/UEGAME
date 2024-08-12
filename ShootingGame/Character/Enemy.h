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

	//���е�������
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=Combat)
		class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		class USoundCue* ImpactSound;

	//ʵʱѪ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
	float Health;

	//���Ѫ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float MaxHealth;

	//��ȡͷ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FString HeadBone;


	//��ʾ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float HealthBarDisplayTime;


	FTimerHandle HealthBarTimer;

	//���ж���
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



	//AI��Ϊ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
	class UBehaviorTree* BehaviorTree;

	//Ѳ�ߵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Behavior Tree",meta=(MakeEditWidget="true"))
	FVector PatrolPoint;

	//Ѳ�ߵ�2
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Behavior Tree", meta = (MakeEditWidget = "true"))
		FVector PatrolPoint2;

	class AEnemyAIController* EnemyController;

	//���˽��й���������
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Combat)
	class USphereComponent* AgroSphere;


	//����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool bStunned;

	//�����εļ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float StunChance;

	//�ڷ�Χ�ڹ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool bInAttackRange;


	//��������Χ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	USphereComponent* CombatRangeSphere;


	//���˹�������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		UAnimMontage* AttackMontage;

	//�ĸ���ͬ�Ĺ�������
	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;


	//����������ײ���
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Combat)
	class UBoxComponent* LeftWeaponCollision;
	
	//����������ײ���
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Combat)
	class UBoxComponent* RightWeaponCollision;


	//���˹����˺�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float BaseDamage;


	//���˹����˺�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FName LeftWeaponSocket;


	//���˹����˺�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	FName RightWeaponSocket;


	//���Թ���
	UPROPERTY(EditAnywhere, Category = Combat)
	bool bCanAttack;

	FTimerHandle AttackWaitTimer;

	//������̵ȴ�ʱ��
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitTime;

	//������������
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

	//ʹ����ͼ�洢��ʾ����Ļ���˺���ֵ
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	//����ʾ����Ļ���˺���ֵ
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	//���˽��й����������ж�,ʹ��UFUNCTION()
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

	//���ŵ��˹�������
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate = 1.0f);



	//���ѡ�񹥻���ʽ
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	//���������ص�
		UFUNCTION()
	void OnLeftWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);


	//���������ص�
		UFUNCTION()
		void OnRightWeaponOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
			);

		//������ͣ����������
		UFUNCTION(BlueprintCallable)
		void ActivateLeftWeapon();
		UFUNCTION(BlueprintCallable)
		void DeactivateLeftWeapon();

		//������ͣ����������
		UFUNCTION(BlueprintCallable)
		void ActivateRightWeapon();
		UFUNCTION(BlueprintCallable)
		void DeactivateRightWeapon();

		//����˺��ĺ���
		void DoDamage(class ADCharacter* Victim);

		//�����Ѫ
		void SpawnBlood(ADCharacter* Victim,FName SocketName);


		//���ν�ɫ
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
