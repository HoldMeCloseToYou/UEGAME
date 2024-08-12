// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include"ShootingGame/DTypes/TurningInPlace.h"
#include"ShootingGame/DTypes/CombatState.h"
#include"Sound/SoundCue.h"
//#include <SoundCueGraph/SoundCueGraph.h>
#include "DCharacter.generated.h"

UCLASS()
class SHOOTINGGAME_API ADCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	//PostInitializeComponents() ��һ���� Actor �� Component ��ʼ����Ϻ󱻵��õ��麯������һ�� Actor �� Component ����������ӵ�������ʱ��
	//���ǵĸ�������ᱻ��ʼ���������е������ʼ����Ϻ�PostInitializeComponents() �����ͻᱻ���á�
	virtual void PostInitializeComponents() override;


	//������̫��
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();


	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	//װ��
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();

	//��׼��ѹ���ͷ�
	void AimButtonPressed();
	void AimButtonReleased();

	float CalculateSpeed();

	//��׼����
	void AimOffset(float DeltaTime);
	void CalcuateAO_Pitch();
	void SimProxiesTurn();

	void FireButtonPressed();
	void FireButtonReleased();

	//����������ť
	void DropButtonPressed();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);


	UFUNCTION(BlueprintCallable)
	void UpdateHUDHealth();

	//����
	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

public:
	UPROPERTY(VisibleAnyWhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnyWhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	//UPROPERTY(EitAnyWhere, BlueprintReadOnly, meta = (AllPrivateAccess = "ture"))
		//class UWidgetComponent OverheadWidget;

	UPROPERTY(VisibleAnyWhere,BlueprintReadOnly)
	class UCombatComponent* Combat;

	UPROPERTY(Replicated)
		class AWeapon* OverlappingWeapon;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	//��ֱ��ˮƽ�������ת��ʹ�䳯��ı�
	float AO_Yaw;
	//��������ת
	float AO_Pitch;

	float InterpAO_Yaw;
	FRotator StartingAimRotation;

	//ת��
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);


	//������̫��

	//����
	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* FireWeaponMontage;


	//����
	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ReloadMontage;
	

	//������
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	//��ɫ��������
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* DeathMontage;



	//��ɫ����
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat)
	bool bDead=false;

	void HideCameraIfCharacterClose();


	//Ѫ��

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Play Stats")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Play Stats")
	float Health = 100.f;

	//���λ���
	UPROPERTY(EditAnywhere)
	float StunChance=0.25f;

	class ADPlayerController* DPlayerController;

	//�����ֵ
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;


	//��ɫ�����к����Ч
	UPROPERTY(EditAnywhere, Category = "CharacterSound")
	class USoundCue* MeleeImpactSound;


	//��ɫ��������Ѫ��Ч��
	UPROPERTY(EditAnywhere, Category = "ParticleSystem")
	UParticleSystem* BloodParticles;

	bool bDeath = false;


public:	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	AWeapon* GetEquippedWeapon();

	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	FVector GetHitTarget();

	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera; }


	FORCEINLINE bool ShouldRotateRootBone()const { return bRotateRootBone; }

	ECombatState GetCombatState()const;

	FORCEINLINE USoundCue* GetMeleeImpactSound()const { return MeleeImpactSound; }


	FORCEINLINE UParticleSystem* GetBloodParticles()const { return BloodParticles; }


	FORCEINLINE bool  isDeath()const { return bDeath; }


};
