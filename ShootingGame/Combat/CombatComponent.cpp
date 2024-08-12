// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include"ShootingGame/Weapon/Weapon.h"
#include"ShootingGame/Character/DCharacter.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"Engine/SkeletalMeshSocket.h"
#include"ShootingGame/HUD/DHUD.h"
#include"ShootingGame/P/DPlayerController.h"
#include"Sound/SoundCue.h"
#include"Kismet/GameplayStatics.h"
#include"Camera/CameraComponent.h"
#include"DrawDebugHelpers.h"
#include"TimerManager.h"
#include"ShootingGame/Character/DCharacter.h"

// ����ս�����,�����д��ս���йص�
UCombatComponent::UCombatComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		//����������ٶ�
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		InitializeCarriedAmmo();
	}

	
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	SetHUDCrosshairs(DeltaTime);
	if (Character)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}

}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr)return;
	bAiming = bIsAiming;
	if (Character)
	{
			Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming?AimWalkSpeed:BaseWalkSpeed;
	}
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SinperRifle)
	{
		Character->ShowSniperScopeWidget(bIsAiming);
	}
	
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		//�����Ļ�ߴ�
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//��Ļ�ռ�
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	//׼��������ռ��е�λ��
	FVector CrosshairWorldPosition;
	//��ʾ׼��������ռ��еķ���
	FVector CrosshairWorldDirection;
	bool bScreenToWorld =UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
		);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;		

		if (Character)
		{
			//
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter+100.f);
		}
		FVector End = Start + CrosshairWorldDirection* TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
			);

		//����Ƿ����
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else 
		{
			HitTarget = TraceHitResult.ImpactPoint;
			/*DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor(0, 0, 0, 0)
			);*/
		}
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr||Character->Controller == nullptr)return;
	//���������Ϊ����ת��ΪpalyerController��
	Controller = Controller == nullptr ? Cast<ADPlayerController>(Character->Controller):Controller;

	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ADHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{

				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;

			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;

			}

			//����ʮ��׼����ɢ
			// 
			// 
			//0-600���ٶ�ӳ�䵽0-1�ķ�Χ
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			//ӳ�䵽���ٶ�
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			//��ȡ��ɫ�ĵ�ǰ�ٶ��������ٶ�������һ����ά����
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
			CrossHairVelocityFactor=FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			//�Ƿ�����Ծ
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrossHairInAirFactor = FMath::FInterpTo(CrossHairInAirFactor,2.25f,DeltaTime,2.25f);
			}
			else
			{
				CrossHairInAirFactor = FMath::FInterpTo(CrossHairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			//�Ƿ�����׼
			if (bAiming)
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrossHairAimFactor = FMath::FInterpTo(CrossHairAimFactor, 0, DeltaTime, 30.f);
			}

			CrossHairShootingFactor = FMath::FInterpTo(CrossHairShootingFactor, 0.f, DeltaTime, 40.f);
			//��ӵ��������У��ƶ��ٶȵ���ɢ������Ծ����ɢ
			HUDPackage.CrosshairSpread =
				0.5f+
				CrossHairVelocityFactor+ 
				CrossHairInAirFactor-
				CrossHairAimFactor+
				CrossHairShootingFactor;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon== nullptr)return;

	//�������׼
	if (bAiming)
	{
		//��һ������ΪĿǰ��fov���ڶ���Ϊ��Ҫ�Ŵ��fov����������ʱ���������ĸ��ǻ�ȡ���Ų�ֵ�ٶȵĺ����������ֵ��Ӱ���ֵ���ٶ�ʹ���Ź��̸���ƽ����
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	} 
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if(Character&&Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if(bFireButtonPressed)
	{
		Fire();
	}

}

void UCombatComponent::Fire()
{
	if (CombatState == ECombatState::ECS_Stunned)return;
	if (EquippedWeapon == nullptr)return;
	
	//ɢ��ǹ���������
	

	if (CanFire())
	{
		
		if (EquippedWeapon)
			{
			CrossHairShootingFactor = 0.75f;
			}

		if (Character && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)
			{
			Character->PlayFireMontage(bAiming);
			EquippedWeapon->Fire(HitTarget);
			CombatState = ECombatState::ECS_Unoccupied;
			StartFireTimer();
			return;
			}

		bCanFire = false;
		if (Character && bFireButtonPressed&& CombatState == ECombatState::ECS_Unoccupied)
			{
			Character->PlayFireMontage(bAiming);
			EquippedWeapon->Fire(HitTarget);
			}

		StartFireTimer();
	}

}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	//SetTimer: ����һ���µĶ�ʱ����
	//TimerHandle: ���ڱ�ʶ��ʱ���ľ����
	//CallbackFunction : Ҫִ�еĻص�������
	//InitialDelay : ��ʱ����ʼִ��ǰ�ĳ�ʼ�ӳ�ʱ�䡣
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
		);

}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed&& EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	//�Զ�����
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr)return false;
	
	//(ɢ��ǹ�������������������ǿգ����ҿ��Կ��𣬲���״̬Ϊ��������ɢ��ǹ������ɢ��ǹ���Կ���
	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun)return true;
	//���ﷵ�ص���ӵ������ ���� ���Կ��� ���� ״̬Ϊ���У����Ҳ����ڻ���״̬
	return !EquippedWeapon->IsEmpty() &&bCanFire&&CombatState==ECombatState::ECS_Unoccupied&& CombatState!= ECombatState::ECS_Stunned;
	
}

void UCombatComponent::Carried_Ammo()
{
	Controller = Controller == nullptr ? Cast<ADPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	//�ж��Ƿ�û�б�������������Ϊɢ��ǹ��״̬�Ƿ�Ϊ����
	bool bJumpToShotgunEnd = CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun && 
		CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}

}

//��ɫ�Դ�ǹе�ĵ�ҩ
void UCombatComponent::InitializeCarriedAmmo()
{
	//Emplace����
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLuncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun,StartingSubmachineGunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun,StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SinperRifle,StartingSniperAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher,StartingGrenadeLauncherAmmo);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{

	if (Character == nullptr||WeaponToEquip==nullptr)return;

	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}
	
	
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
	
}

void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SwapWeapons Start"));
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);


	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(SecondaryWeapon);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("finish AttachActorToBackpack"));
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	//�����ӵ�
	EquippedWeapon->SetHUDAmmo();
	//����Я�����ӵ�
	UpdateCarriedAmmo();
	//������������
	PlayEquipWeaponSound(EquippedWeapon);

	
}

void UCombatComponent::DropEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

void UCombatComponent::On_EquipSecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
		AttachActorToBackpack(SecondaryWeapon);
		PlayEquipWeaponSound(EquippedWeapon);
	}

}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)return;
	

	DropEquippedWeapon();
	//��������Ϊ����װ��������
	EquippedWeapon = WeaponToEquip;

	//����˵���Ѿ�װ��
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	//��������UI
	EquippedWeapon->ShowPickupWidget(false);

	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	//����������
	EquippedWeapon->SetOwner(Character);
	//�����ӵ�
	EquippedWeapon->SetHUDAmmo();

	//Contain���ڼ�飬����Ƿ����WeaponType()
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	//����Я�����ӵ�
	Carried_Ammo();

	//������������
	PlayEquipWeaponSound(WeaponToEquip);

	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(WeaponToEquip);
	//������������
	PlayEquipWeaponSound(WeaponToEquip);
	SecondaryWeapon->SetOwner(Character);

}

void UCombatComponent::AttachActorToBackpack(AActor* ActorToAttach)
{
	if (Character == nullptr || Character->GetMesh()==nullptr || ActorToAttach == nullptr)return;
	const USkeletalMeshSocket* BackpackSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (BackpackSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("BackpackSocket"));
		BackpackSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}

}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
}

void UCombatComponent::PlayEquipWeaponSound(AWeapon* WeaponToEquip)
{

	if (Character&& WeaponToEquip->EquipSound&&WeaponToEquip)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponToEquip->EquipSound,
			//���ŵ�λ��
			Character->GetActorLocation()
		);
	}
}

void UCombatComponent::Reload()
{
	//����ӵ������ľͲ�reload
	if (EquippedWeapon->GetAmmo() == EquippedWeapon->GetMagCapacity())return;

	if (CarriedAmmo > 0&&CombatState!=ECombatState::ECS_Reloading)
	{
	
		if (Character == nullptr||EquippedWeapon==nullptr)return;

		
		CombatState = ECombatState::ECS_Reloading;
		HandleReload();

	}
}

void UCombatComponent::HandleReload()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("play montage"));
	Character->PlayReloadMontage();
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr)return 0;

	//RoomInWeapon��װ����ʣ�µ�
	int32 RoomInMag=EquippedWeapon->GetMagCapacity()-EquippedWeapon->GetAmmo();
	
	//����Ƿ�������
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	
	return 0;
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr)return;
	
	CombatState = ECombatState::ECS_Unoccupied;
	UpdateAmmoValues();
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::ShotgunShellReload()
{
	if (Character)
	{
		UpdateShotgunAmmoValues();
	}
	

}



void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	//���������������
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("CarriedAmmoMap.Contains(WeaponType)"));
		//��ӵ��ӵ��������������
		CarriedAmmoMap[WeaponType] =FMath::Clamp(CarriedAmmoMap[WeaponType]+AmmoAmount,0,MaxCarriedAmmo);
		

		UpdateCarriedAmmo();
	}
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

void UCombatComponent::Combat_State()
{
	switch(CombatState)
	{

	case ECombatState::ECS_Reloading:
		HandleReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr||EquippedWeapon==nullptr)return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Carried_Ammo();
	EquippedWeapon->AddAmmo(-ReloadAmount);
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ADPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr)return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Carried_Ammo();
	EquippedWeapon->AddAmmo(-1);
	bCanFire = true;
	if (EquippedWeapon->IsFull()||CarriedAmmo==0)
	{
		//��ת��ɢ��ǹ��������
		JumpToShotgunEnd();
	}
}

void UCombatComponent::JumpToShotgunEnd()
{
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->ReloadMontage)
	{

		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));
		
	}
}

void UCombatComponent::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
	if (Character->IsAiming())
	{
		SetAiming(true);
	}

}

void UCombatComponent::Stun()
{
	if (Character->Health <= 0.f)return;
	CombatState = ECombatState::ECS_Stunned;

	if (Character)
	{
		Character->PlayHitReactMontage();
	}
}

