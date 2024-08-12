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


	//��������
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//����pawn����ײ������ֱ���߹�ȥ
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//�Զ������ģ��
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	//��Ǹ�����ģ�͵���Ⱦ״̬��Ҫ����
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
	//�رո߹�
	EnableCustomDepth(false);
	//��������
	WeaponMesh->SetSimulatePhysics(false);
	//��������
	WeaponMesh->SetEnableGravity(false);
	//������ײ
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//����ǳ��ǹ������������
	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}
}

void AWeapon::OnDropped()
{
	//�Զ������ģ��
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	//��Ǹ�����ģ�͵���Ⱦ״̬��Ҫ����
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);


	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetSimulatePhysics(true);
	//��������
	WeaponMesh->SetEnableGravity(true);
	//������ײ
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//��������
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//����pawn����ײ������ֱ���߹�ȥ
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AWeapon::OnEquippedSecondary()
{
	//�رո߹�
	EnableCustomDepth(false);
	ShowPickupWidget(false);
	//��������
	WeaponMesh->SetSimulatePhysics(false);
	//��������
	WeaponMesh->SetEnableGravity(false);
	//������ײ
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//����ǳ��ǹ������������
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
	//������ʽ���ȼ��� Ammo - 1��Ȼ�󽫽�������ڷ�Χ [0, MagCapacity] �ڡ�
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

			//�����GetSocketTransform��Ҫsocket�����ĹǼ�����
			FTransform SocketTransform = AmmoEjectFlashSocket->GetSocketTransform(WeaponMesh);

			if (World)
			{
				World->SpawnActor<ACasing>(
					//������
					CasingClass,
					//��ȡλ��
					SocketTransform.GetLocation(),
					//��Ҫƫ�Ƶ�λ��
					SocketTransform.GetLocation().Rotation()
					//SpawnParams ��һ���ṹ�壬����ָ�����ɵ���ʱ�ĸ��ֲ���
				);
			}

		}
	}
	SpendRound();
}

//�ӵ��Ƿ�Ϊ��
bool AWeapon::IsEmpty()
{
	return Ammo<=0;
}

bool AWeapon::IsFull()
{
	//�����ӵ��Ƿ�����
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
	//Ammo����ǰ�ĵ�ҩ�� AmmoToAdd����ӵ���ǰ��ҩ����ֵ MagCapacity:��ʾ��ϻ���������ޡ�
	Ammo = FMath::Clamp(Ammo - AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
}



