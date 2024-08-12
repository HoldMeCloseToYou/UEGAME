// Fill out your copyright notice in the Description page of Project Settings.


#include "DAnimInstance.h"
#include"DCharacter.h"
#include"Kismet/KismetMathLibrary.h"
//��ɫ�ƶ����
#include"GameFramework/CharacterMovementComponent.h"
#include"ShootingGame\Weapon\ProjectileWeapon.h"
#include"ShootingGame/Weapon/Weapon.h"
#include"ShootingGame/DTypes/CombatState.h"

void UDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//TryGetPawnOwner���ӵ����Pawn�����ص���ӵ���������ʵ����Pawn
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


	//��ȡ�ٶ�
	FVector Veocity = DCharacter->GetVelocity();

	Veocity.Z = 0.f;
	Speed = Veocity.Size();

	//������Ҫ�����ɫ�ƶ����
	bIsInAir = DCharacter->GetCharacterMovement()->IsFalling();

	//����ʹ������Ԫ��������򻯴���
	bIsAccelerating = DCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = DCharacter->IsWeaponEquipped();
	EquippedWeapon = DCharacter->GetEquippedWeapon();
	bIsCrouched = DCharacter->bIsCrouched;

	//bAiming���ڽ�ɫ��IsAiming
	bAiming = DCharacter->IsAiming();

	//ԭ��ת��ΪDCharacter��ת��
	TurningInPlace = DCharacter->GetTurningInPlace();

	bRotateRootBone = DCharacter->ShouldRotateRootBone();

	bDeath = DCharacter->isDeath();

	//��õ������ƶ��ķ���������ǿ������ķ���
	FRotator AimRotation = DCharacter->GetBaseAimRotation();

	//���д��뿴������ Unreal Engine �е� C++ ���룬���������Ǵ���һ�� FRotator ���͵ı��� MovementRotation
	//������ʾ�������ת����ʹ���� UKismetMathLibrary �е� MakeRotFromX ������
	//�ú������ݸ����� X �᷽��ͨ����������ٶȷ��򣩴���һ����ת��
	//�����DCharacter->GetVelocity() �����˽�ɫ���ٶ�������Ȼ�� MakeRotFromX �������ݸ��ٶ���������һ����ת��
	//ʹ�� X �����ٶ�����һ�£��Ӷ�ʵ���˳����ٶȷ������ת��
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(DCharacter->GetVelocity());


	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);

	//�����Ĳ�������ʹ��ת���̸���ƽ������������Ϸ�п����������תʱ������ʹ�ò�ֵ��ʹ��ת����ƽ����Ȼ������ͻȻ����ת�仯
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

	

	//��ȡ�����ϵģ����ֵĲ��
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
		

		//���ǹ�ܵ�λ��
		FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f,FColor::Red);
		//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(),DCharacter->GetHitTarget(), FColor::Orange);

	}

	bUseFABRIK = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAimOffsets = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = DCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}
