// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "Components/BoxComponent.h"
#include"WeaponTypes.h"
#include "Weapon.generated.h"

//武器状态
UENUM(blueprintType)
enum class EWeaponState : uint8
{
	//初始状态
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
	//装备武器的状态
	virtual void OnEquipped();
	//丢掉武器的状态
	virtual void OnDropped();
	//装备第二把武器
	virtual void OnEquippedSecondary();




	UFUNCTION()
	virtual void OnSphereOverlap(
		//重叠
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


	//武器名字
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Weapon ProPerties")
	FString GunName= "Default";


	//允许在编辑器中选择一个继承自 ACasing 类的子类，并将所选的类赋值给 CasingClass 属性。
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing>CasingClass;

	//子弹
	UPROPERTY(EditAnywhere)
	int32 Ammo;

	//容量
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	void SpendRound();


	UPROPERTY()
	class ADCharacter* DOwnerCharacter;
	UPROPERTY()
	class ADPlayerController* DOwnerController;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;



	//武器十字准线

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

	//右键放大
	UPROPERTY(EditAnywhere)
		float ZoomFOV = 30.f;

	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed = 20.f;


	//自动开火和开火延迟
	UPROPERTY(EditAnywhere, Category = Combat)
		float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
		bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
		class USoundCue* EquipSound;

	void  EnableCustomDepth(bool bEnable);

	//伤害
	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 20.f;

	//击中头部伤害
	UPROPERTY(EditAnywhere, Category = "Damage")
		float HeadShotDamage = 50.f;

public:	
	//使用FORCEINLINE来标记那些频繁调用且逻辑简单的函数，以提高程序的执行效率。在C++中与函数声明一起使用
	//设置武器状态
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere()const { return AreaSphere; }
	//获取武器网格
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
