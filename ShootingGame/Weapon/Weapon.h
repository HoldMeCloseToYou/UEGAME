// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include"WeaponTypes.h"
#include "Weapon.generated.h"

//����״̬
UENUM(blueprintType)
enum class EWeaponState : uint8
{
	//��ʼ״̬
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SHOOTINGGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector&HitTarget);
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);

protected:

	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	//װ��������״̬
	virtual void OnEquipped();
	//����������״̬
	virtual void OnDropped();
	//װ���ڶ�������
	virtual void OnEquippedSecondary();




	UFUNCTION()
	virtual void OnSphereOverlap(
		//�ص�
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OhterBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OhterBodyIndex
		);


public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon ProPerties")
		USkeletalMeshComponent* WeaponMesh;


	UPROPERTY(VisibleAnywhere, Category = "Weapon ProPerties")
		class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere,Category = "Weapon ProPerties")
		EWeaponState WeaponState;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Weapon ProPerties")
		class UWidgetComponent* PickUpWidget;
	
	UPROPERTY(EditAnyWhere,Category="Weapon Properties")
	class UAnimationAsset* FireAnimation;


	//��������
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Weapon ProPerties")
	FString GunName= "Default";


	//�����ڱ༭����ѡ��һ���̳��� ACasing ������࣬������ѡ���ำֵ�� CasingClass ���ԡ�
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing>CasingClass;

	//�ӵ�
	UPROPERTY(EditAnywhere)
	int32 Ammo;

	//����
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	void SpendRound();


	UPROPERTY()
	class ADCharacter* DOwnerCharacter;
	UPROPERTY()
	class ADPlayerController* DOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;



	//����ʮ��׼��

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsBottom;

	//�Ҽ��Ŵ�
	UPROPERTY(EditAnywhere)
		float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;


	//�Զ�����Ϳ����ӳ�
	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
		class USoundCue* EquipSound;

	void  EnableCustomDepth(bool bEnable);

	//�˺�
	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 20.f;

	//����ͷ���˺�
	UPROPERTY(EditAnywhere, Category = "Damage")
		float HeadShotDamage = 50.f;

public:	
	//ʹ��FORCEINLINE�������ЩƵ���������߼��򵥵ĺ���������߳����ִ��Ч�ʡ���C++���뺯������һ��ʹ��
	//��������״̬
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere()const { return AreaSphere; }
	//��ȡ��������
	FORCEINLINE	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE	float GetZoomFOV() const { return ZoomFOV; }
	FORCEINLINE	float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponType GetWeaponType()const { return WeaponType; }


	FORCEINLINE int32 GetAmmo()const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity()const { return MagCapacity; }

	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }


};
