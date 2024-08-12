// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
//����MuzzleFlashSocket
#include"Engine/SkeletalMeshSocket.h"
#include"projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	//�������ڲ����䵯
	Super::Fire(HitTarget);
	//�����Ȼ�ȡ�󣬽��ź����if�����ж��Ƿ���Ч
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	//USkeletalMeshSocket ��ͨ�����ڱ�ʾ��������ģ���е��ض�λ��
	const USkeletalMeshSocket* MuzzleFlashSocket =GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	
	if (MuzzleFlashSocket&&World)
	{
		
		//�����GetSocketTransform��Ҫsocket�����ĹǼ�����
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		//��ǹ�ڼ�˵�����ɫ׼�ǻ��е�λ��
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		//����������ָ������ת��Ϊ��ת�Ƕ�
		FRotator TargetRotation = ToTarget.Rotation();


		//�������projectclass���ӵ��ͽ�ɫ����Ч��
		if (ProjectClass&&InstigatorPawn)
		{

		
			FActorSpawnParameters SpawnParams;
			//���÷��������Ľ�ɫ�ǵ����ӵ����
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			Aprojectile* SpawnedProjectile = nullptr;
			if (World)
			{
				World->SpawnActor<Aprojectile>(
					//������
					ProjectClass,
					//��ȡλ��
					SocketTransform.GetLocation(),
					//��Ҫƫ�Ƶ�λ��
					TargetRotation,
					//SpawnParams ��һ���ṹ�壬����ָ�����ɵ���ʱ�ĸ��ֲ���
					SpawnParams
				);
			}

		}
	}
}
