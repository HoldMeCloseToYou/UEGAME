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
	//��������
	void SwapWeapons();


	void DropEquippedWeapon();

	void On_EquipSecondaryWeapon();

	void Reload();

	void HandleReload();

	int32 AmountToReload();

	//����ͼ��ʹ�øú���
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

	//������Ϣ����
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	/*UFUNCTION()
		void ServerReload();*/

	//ʮ��׼���ƶ���ӳ��
	float CrossHairVelocityFactor;
	//ʮ��׼����Ծ��ӳ��
	float CrossHairInAirFactor;
	//ʮ��׼�ߵ���ɢ
	float CrossHairAimFactor;
	//ʮ��׼�ߵ������ɢ
	float CrossHairShootingFactor;
public:	
	bool bAiming;
	bool bFireButtonPressed;

	UPROPERTY(EditAnyWhere)
		float BaseWalkSpeed;
	UPROPERTY(EditAnyWhere)
		float AimWalkSpeed;

	FVector HitTarget;

	//û����׼��ʱ�����Ұ
	float DefaultFOV;



	UPROPERTY(EditAnyWhere,Category=Combat)
	float ZoomedFOV = 30.f;
	
	float CurrentFOV;

	//�����ٶ�
	UPROPERTY(EditAnyWhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);


	//�Զ�����
	FTimerHandle FireTimer;

	bool bCanFire = true;
	//������ʱ��(�Զ�����)
	void StartFireTimer();
	//��ɿ���Ļص�
	void FireTimerFinished();

	
	bool CanFire();

	UPROPERTY(EditAnywhere)
	int32 CarriedAmmo;

	UFUNCTION()
	void Carried_Ammo();


	//ʹ��TMap���������д����������ͣ����ӵ�
	TMap<EWeaponType, int32>CarriedAmmoMap;

	//���Я����ҩ
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;

	//��ʼ��AR��ҩ
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 30;

	//��ʼ�������ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingRocketAmmo = 0;

	//��ʼ����ǹ��ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo = 0;

	//��ʼ�����ǹ��ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingSubmachineGunAmmo = 0;

	//��ʼ��ɢ��ǹ��ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingShotgunAmmo = 0;

	//��ʼ���ѻ�ǹ��ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingSniperAmmo = 0;


	//��ʼ���񵯵�ҩ
	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo = 0;



	//��ʼ���Լ�Я�������ӵ���ʼ��Ϸ
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

	//װ��������
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	//װ��������
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	void AttachActorToBackpack(AActor* ActorToAttach);

	bool ShouldSwapWeapons();

};
