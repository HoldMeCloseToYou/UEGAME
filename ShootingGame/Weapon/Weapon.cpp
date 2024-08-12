// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include"Components/SphereComponent.h"
#include"Components/WidgetComponent.h "
#include"ShootingGame/Character/DCharacter.h"
#include"Animation/AnimationAsset.h"
#include"Components/SkeletalMeshComponent.h"
#include"Casing.h"
#include"ShootingGame/P/DPlayerController.h"
#include"ShootingGame/Combat/CombatComponent.h"
#include"Engine/SkeletalMeshSocket.h"

AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);


	//设置物理
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//忽略pawn的碰撞，可以直接走过去
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//自定义深度模板
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	//标记该武器模型的渲染状态需要更新
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	  
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, & AWeapon::OnSphereEndOverlap);
		if (PickUpWidget)
		{
			PickUpWidget->SetVisibility(false);
		}

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;

	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}

}

void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnWeaponStateSet();

}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);
	//关闭高光
	EnableCustomDepth(false);
	//设置物理
	WeaponMesh->SetSimulatePhysics(false);
	//设置重力
	WeaponMesh->SetEnableGravity(false);
	//设置碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//如果是冲锋枪，则启动物理
	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AWeapon::OnDropped()
{
	//自定义深度模板
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	//标记该武器模型的渲染状态需要更新
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);


	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetSimulatePhysics(true);
	//设置重力
	WeaponMesh->SetEnableGravity(true);
	//设置碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//设置物理
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//忽略pawn的碰撞，可以直接走过去
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AWeapon::OnEquippedSecondary()
{
	//关闭高光
	EnableCustomDepth(false);
	ShowPickupWidget(false);
	//设置物理
	WeaponMesh->SetSimulatePhysics(false);
	//设置重力
	WeaponMesh->SetEnableGravity(false);
	//设置碰撞
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//如果是冲锋枪，则启动物理
	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADCharacter* DCharacter = Cast <ADCharacter>(OtherActor);
	if (DCharacter)
	{
		DCharacter->SetOverlappingWeapon(this);
	}


}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex)
{
	ADCharacter* DCharacter = Cast <ADCharacter>(OtherActor);
	if (DCharacter)
	{
		DCharacter->SetOverlappingWeapon(nullptr);
	}

}

void AWeapon::SpendRound()
{
	//这个表达式首先计算 Ammo - 1，然后将结果限制在范围 [0, MagCapacity] 内。
	Ammo=FMath::Clamp(Ammo-1,0,MagCapacity);
	SetHUDAmmo();
}

void AWeapon::SetHUDAmmo()
{
	DOwnerCharacter = DOwnerCharacter == nullptr ? Cast<ADCharacter>(GetOwner()) : DOwnerCharacter;
	if (DOwnerCharacter && DOwnerCharacter->Combat && IsFull())
	{
		DOwnerCharacter->Combat->JumpToShotgunEnd();
	}
	if (DOwnerCharacter)
	{
		DOwnerController = DOwnerController == nullptr ? Cast<ADPlayerController>(DOwnerCharacter->Controller) : DOwnerController;
		if (DOwnerController)
		{
			DOwnerController->SetHUDWeaponAmmo(Ammo);
		}

	}
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	DOwnerCharacter = DOwnerCharacter = nullptr ? Cast<ADCharacter>(Owner) : DOwnerCharacter;
	if (DOwnerCharacter && DOwnerCharacter->GetEquippedWeapon() && DOwnerCharacter->GetEquippedWeapon() == this)
	{
		SetHUDAmmo();
	}
	
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectFlashSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		UWorld* World = GetWorld();

		if (AmmoEjectFlashSocket)
		{

			//这里的GetSocketTransform需要socket所属的骨架网格
			FTransform SocketTransform = AmmoEjectFlashSocket->GetSocketTransform(WeaponMesh);

			if (World)
			{
				World->SpawnActor<ACasing>(
					//弹丸类
					CasingClass,
					//获取位置
					SocketTransform.GetLocation(),
					//需要偏移的位置
					SocketTransform.GetLocation().Rotation()
					//SpawnParams 是一个结构体，用于指定生成弹丸时的各种参数
				);
			}

		}
	}
	SpendRound();
}

//子弹是否为空
bool AWeapon::IsEmpty()
{
	return Ammo<=0;
}

bool AWeapon::IsFull()
{
	//返回子弹是否满的
	return Ammo==MagCapacity;
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	DOwnerCharacter = nullptr;
	DOwnerController = nullptr;
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	//Ammo：当前的弹药量 AmmoToAdd：添加到当前弹药量的值 MagCapacity:表示弹匣的容量上限。
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}



