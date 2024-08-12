// Fill out your copyright notice in the Description page of Project Settings.


#include "DAnimInstance.h"
#include"DCharacter.h"
#include"Kismet/KismetMathLibrary.h"
//角色移动组件
#include"GameFramework/CharacterMovementComponent.h"
#include"ShootingGame\Weapon\ProjectileWeapon.h"
#include"ShootingGame/Weapon/Weapon.h"
#include"ShootingGame/DTypes/CombatState.h"

void UDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//TryGetPawnOwner获得拥有者Pawn，返回的是拥有这个动画实例的Pawn
	DCharacter = Cast<ADCharacter>(TryGetPawnOwner());
	
}

void UDAnimInstance::NativeUpdateAnimation(float DeltaTime)
{

	Super::NativeUpdateAnimation(DeltaTime);

	if (DCharacter == nullptr)
	{
		DCharacter = Cast<ADCharacter>(TryGetPawnOwner());
	}
	if (DCharacter == nullptr)return;


	//获取速度
	FVector Veocity = DCharacter->GetVelocity();

	Veocity.Z = 0.f;
	Speed = Veocity.Size();

	//这里需要引入角色移动组件
	bIsInAir = DCharacter->GetCharacterMovement()->IsFalling();

	//这里使用了三元运算符来简化代码
	bIsAccelerating = DCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = DCharacter->IsWeaponEquipped();
	EquippedWeapon = DCharacter->GetEquippedWeapon();
	bIsCrouched = DCharacter->bIsCrouched;

	//bAiming等于角色的IsAiming
	bAiming = DCharacter->IsAiming();

	//原地转身为DCharacter的转身
	TurningInPlace = DCharacter->GetTurningInPlace();

	bRotateRootBone = DCharacter->ShouldRotateRootBone();

	bDeath = DCharacter->isDeath();

	//获得的正在移动的方向或者我们控制器的方向
	FRotator AimRotation = DCharacter->GetBaseAimRotation();

	//这行代码看起来是 Unreal Engine 中的 C++ 代码，它的作用是创建一个 FRotator 类型的变量 MovementRotation
	//用来表示物体的旋转。它使用了 UKismetMathLibrary 中的 MakeRotFromX 函数，
	//该函数根据给定的 X 轴方向（通常是物体的速度方向）创建一个旋转。
	//在这里，DCharacter->GetVelocity() 返回了角色的速度向量，然后 MakeRotFromX 函数根据该速度向量创建一个旋转，
	//使得 X 轴与速度向量一致，从而实现了朝向速度方向的旋转。
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(DCharacter->GetVelocity());


	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);

	//这样的操作用于使旋转过程更加平滑，比如在游戏中控制物体的旋转时，可以使用插值来使旋转更加平滑自然，避免突然的旋转变化
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = DCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);


	AO_Yaw = DCharacter->GetAO_Yaw();
	AO_Pitch = DCharacter->GetAO_Pitch();

	

	//获取武器上的，左手的插槽
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && DCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		DCharacter->GetMesh()->TransformToBoneSpace(FName("RightHand"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));


		FTransform MuzzleTipTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);




		bLocallyControlled = true;
		FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("RightHand"), ERelativeTransformSpace::RTS_World);
		//FVector RightHandTransform90(-90.f, 0, 0);
		//RightHandRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), DCharacter->GetHitTarget());
		//RightHandRotation=UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(),RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - DCharacter->GetHitTarget()));
		

		//获得枪管的位置
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f,FColor::Red);
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(),DCharacter->GetHitTarget(), FColor::Orange);

	}

	bUseFABRIK = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}
