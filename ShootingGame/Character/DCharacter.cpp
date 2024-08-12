// Fill out your copyright notice in the Description page of Project Settings.


#include "DCharacter.h"
#include"GameFramework/SpringArmComponent.h"
#include"Camera/CameraComponent.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"ShootingGame/Combat/CombatComponent.h"
#include"ShootingGame/Weapon/Weapon.h"
#include"Kismet/KismetMathLibrary.h"
#include"ShootingGame/P/DPlayerController.h"
#include"ShootingGame/Weapon/WeaponTypes.h"
#include"Kismet/GameplayStatics.h"
#include"Enemy.h"
#include"EnemyAIController.h"
#include"DAnimInstance.h"
#include"BehaviorTree/BlackboardComponent.h"


// Sets default values
ADCharacter::ADCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	//摄像臂长度
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	//使得角色不是和控制器一起旋转
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;


	//创建一个名为"CombatComponent"的默认子对象，并将其作为一个指针或引用赋值给名为"Combat"的变量。
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	//Combat = SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	TurningInPlace=ETurningInPlace::ETIP_NotTurning;                                                                                               
}


void ADCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateHUDHealth();
	//OnTakeAnyDamage：这是一个 Unreal Engine 中内置的事件，当角色受到任何伤害时就会触发
	OnTakeAnyDamage.AddDynamic(this, &ADCharacter::ReceiveDamage);
}


void ADCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy)
	{
		AimOffset(DeltaTime);
	}
	else
	{
		SimProxiesTurn();
	}
	HideCameraIfCharacterClose();

}

void ADCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DORELIFETIME(ADCharacter, OverlappingWeapon);

}

void ADCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		//AddMovementInput函数只获取方向和值并使角色行动
		AddMovementInput(Direction, Value);
	}
}

void ADCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		//AddMovementInput函数只获取方向和值并使角色行动
		AddMovementInput(Direction, Value);
	}
}

void ADCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ADCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ADCharacter::EquipButtonPressed()
{

	if (Combat)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("equipButtonPressed"));
		if (OverlappingWeapon)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EquipWeapon(OverlappingWeapon)"));
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("should Swap"));
			Combat->SwapWeapons();
		}
	}

}

void ADCharacter::CrouchButtonPressed()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	};

}

void ADCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}

void ADCharacter::AimButtonPressed()
{
	if (Combat)
	{

		Combat->SetAiming(true);
	}
}

void ADCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat-> SetAiming(false);
	}
}

float ADCharacter::CalculateSpeed()
{
	FVector Veocity = GetVelocity();
	Veocity.Z = 0.f;
	return Veocity.Size();
}

void ADCharacter::AimOffset(float DeltaTime)
{
	
	if (Combat && Combat->EquippedWeapon == nullptr)return;
	//获取速度
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)	//没有动，也没有跳跃
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation= FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		//（旋转的起始值，旋转的当前值），这里调换了是因为旋转方向出错
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		bUseControllerRotationYaw = true;
		//如果没有旋转
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}

		TurnInPlace(DeltaTime);
	}

	if (Speed > 0.f || bIsInAir)	//正在移动或者是在跳跃
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw =0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}


	CalcuateAO_Pitch();
}

void ADCharacter::CalcuateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f)
	{
		//将 AO_Pitch 角度从输入范围 (270.f, 360.f) 映射到输出范围 (-90.f, 0.f)。
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueUnclamped(InRange, OutRange, AO_Pitch);
	}
}

void ADCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	bRotateRootBone = false;
	float Speed=CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	CalcuateAO_Pitch();
}

void ADCharacter::TurnInPlace(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw:%f"), AO_Yaw);
	//向右边转超过90°
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	//如果正在处于转身的时候
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		//将坐标重置为0
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}

}

void ADCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ADCharacter::FireButtonReleased()
{

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

//丢弃武器
void ADCharacter::DropButtonPressed()
{
	if (IsWeaponEquipped())
	{
		GetEquippedWeapon()->Dropped();
	}
}

//受到伤害函数
void ADCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCause)
{
	//(扣除后的血量,最低范围，最大范围）
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	if (Health <= 0.f)
	{
		Die();
		auto EnemyController = Cast<AEnemyAIController>(InstigatorController);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CharacterDead"), true);
		}
	}
}

void ADCharacter::UpdateHUDHealth()
{
	DPlayerController = DPlayerController == nullptr ? Cast<ADPlayerController>(Controller) : DPlayerController;
	if (DPlayerController)
	{
		DPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ADCharacter::Die()
{
	bDeath = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayDieMontage"));
	}
}

void ADCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
}

void ADCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ADCharacter::HideCameraIfCharacterClose()
{

	//摄像机位置减去当前角色位置的大小小于自己设定的一个值（CameraThreshold)
	if ((FollowCamera->GetComponentLocation()-GetActorLocation()).Size()<CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon&&Combat->EquippedWeapon->GetWeaponMesh())
		{
			//这会设置只会对自己不可视
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee=true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{

			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}

}

void ADCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{

	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}
// 按键绑定
void ADCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//移动
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis("MoveForward", this, &ADCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ADCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ADCharacter::LookUp);


	//装备
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ADCharacter::EquipButtonPressed);

	//丢弃
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ADCharacter::DropButtonPressed);

	//蹲下
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADCharacter::CrouchButtonPressed);

	//瞄准
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ADCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ADCharacter::AimButtonReleased);

	//射击
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ADCharacter::FireButtonReleased);

	//换弹
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ADCharacter::ReloadButtonPressed);


}


void ADCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}

}

//播放开火动画
void ADCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PlayFireMontage"));
	}
}

//播放换弹动画
void ADCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr)return;
	//取角色网格组件关联的动画实例的函数
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		
		FName SectionName;
		
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLuncher:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SinperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Montage_Play(ReloadMontage);"));
	}
}

void ADCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr)return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(FName("Hit_1"));
	}
}

bool ADCharacter::IsWeaponEquipped()
{
	return (Combat&&Combat->EquippedWeapon);
}

bool ADCharacter::IsAiming()
{
	
	return (Combat && Combat->bAiming);	
}

AWeapon* ADCharacter::GetEquippedWeapon()
{
	//如果战斗为空，则退出，
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector ADCharacter::GetHitTarget() 
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}


ECombatState ADCharacter::GetCombatState() const
{
	//如果战斗为空返回
	if (Combat == nullptr)return ECombatState::ECS_MAX;
	return Combat->CombatState;
}


