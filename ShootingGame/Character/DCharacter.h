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

	//PostInitializeComponents() 是一个在 Actor 或 Component 初始化完毕后被调用的虚函数。当一个 Actor 或 Component 被创建并添加到世界中时，
	//它们的各种组件会被初始化。当所有的组件初始化完毕后，PostInitializeComponents() 函数就会被调用。
	virtual void PostInitializeComponents() override;


	//播放蒙太奇
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
	//装备
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();

	//瞄准按压和释放
	void AimButtonPressed();
	void AimButtonReleased();

	float CalculateSpeed();

	//瞄准动作
	void AimOffset(float DeltaTime);
	void CalcuateAO_Pitch();
	void SimProxiesTurn();

	void FireButtonPressed();
	void FireButtonReleased();

	//丢弃武器按钮
	void DropButtonPressed();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);


	UFUNCTION(BlueprintCallable)
	void UpdateHUDHealth();

	//死亡
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

	//垂直于水平面的轴旋转，使其朝向改变
	float AO_Yaw;
	//俯仰的旋转
	float AO_Pitch;

	float InterpAO_Yaw;
	FRotator StartingAimRotation;

	//转身
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);


	//动画蒙太奇

	//开火
	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* FireWeaponMontage;


	//换弹
	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ReloadMontage;
	

	//被击中
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	//角色死亡动画
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* DeathMontage;



	//角色死亡
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Combat)
	bool bDead=false;

	void HideCameraIfCharacterClose();


	//血量

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Play Stats")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Play Stats")
	float Health = 100.f;

	//击晕机会
	UPROPERTY(EditAnywhere)
	float StunChance=0.25f;

	class ADPlayerController* DPlayerController;

	//相机阈值
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;


	//角色被击中后的音效
	UPROPERTY(EditAnywhere, Category = "CharacterSound")
	class USoundCue* MeleeImpactSound;


	//角色被击中流血的效果
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
