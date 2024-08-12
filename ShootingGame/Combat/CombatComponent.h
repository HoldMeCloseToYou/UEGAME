// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include"ShootingGame/Weapon/WeaponTypes.h"
#include"ShootingGame/DTypes/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTINGGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	friend class ADCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
	//交换武器
	void SwapWeapons();


	void DropEquippedWeapon();

	void On_EquipSecondaryWeapon();

	void Reload();

	void HandleReload();

	int32 AmountToReload();

	//在蓝图中使用该函数
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void PlayEquipWeaponSound(AWeapon*WeaponToEquip);

	void PickupAmmo(EWeaponType WeaponType,int32 AmmoAmount);

private:
	class ADCharacter* Character;

	class ADPlayerController* Controller;

	class ADHUD* HUD;

	class  AWeapon* EquippedWeapon;

	UPROPERTY()
	AWeapon* SecondaryWeapon;


protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	void FireButtonPressed(bool bPressed);

	void Fire();

	//命中信息返回
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	/*UFUNCTION()
		void ServerReload();*/

	//十字准线移动的映射
	float CrossHairVelocityFactor;
	//十字准线跳跃的映射
	float CrossHairInAirFactor;
	//十字准线的扩散
	float CrossHairAimFactor;
	//十字准线的射击扩散
	float CrossHairShootingFactor;
public:	
	bool bAiming;
	bool bFireButtonPressed;

	UPROPERTY(EditAnyWhere)
		float BaseWalkSpeed;
	UPROPERTY(EditAnyWhere)
		float AimWalkSpeed;

	FVector HitTarget;

	//没有瞄准的时候的视野
	float DefaultFOV;



	UPROPERTY(EditAnyWhere,Category=Combat)
	float ZoomedFOV = 30.f;
	
	float CurrentFOV;

	//缩放速度
	UPROPERTY(EditAnyWhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);


	//自动开火
	FTimerHandle FireTimer;

	bool bCanFire = true;
	//启动计时器(自动开火)
	void StartFireTimer();
	//完成开火的回调
	void FireTimerFinished();

	
	bool CanFire();

	UPROPERTY(EditAnywhere)
	int32 CarriedAmmo;

	UFUNCTION()
	void Carried_Ammo();


	//使用TMap容器，进行储存武器类型，和子弹
	TMap<EWeaponType, int32>CarriedAmmoMap;

	//最大携带弹药
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	//初始化AR弹药
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	//初始化火箭弹药
	UPROPERTY(EditAnywhere)
		int32 StartingRocketAmmo = 0;

	//初始化手枪弹药
	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo = 0;

	//初始化冲锋枪弹药
	UPROPERTY(EditAnywhere)
		int32 StartingSubmachineGunAmmo = 0;

	//初始化散弹枪弹药
	UPROPERTY(EditAnywhere)
		int32 StartingShotgunAmmo = 0;

	//初始化狙击枪弹药
	UPROPERTY(EditAnywhere)
		int32 StartingSniperAmmo = 0;


	//初始化榴弹弹药
	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo = 0;



	//初始化自己携带多少子弹开始游戏
	void InitializeCarriedAmmo();


	UPROPERTY(EditAnywhere)
	ECombatState CombatState=ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void Combat_State();

	void UpdateAmmoValues();

	void UpdateCarriedAmmo();

	void UpdateShotgunAmmoValues();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void EndStun();


	void Stun();

	//装备主武器
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	//装备副武器
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	void AttachActorToBackpack(AActor* ActorToAttach);

	bool ShouldSwapWeapons();

};
