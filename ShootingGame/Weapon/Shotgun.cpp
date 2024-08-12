// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include"Engine/SkeletalMeshSocket.h"
#include"Kismet/GamePlayStatics.h"
#include"NiagaraFunctionLibrary.h"
#include"NiagaraComponent.h"
#include"Sound/SoundCue.h"
#include"Particles/ParticleSystemComponent.h"
#include"ShootingGame/Character/Enemy.h"
#include"ShootingGame/Character/DCharacter.h"
#include"ShootingGame/Explosive/Explosive.h"
void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)return;

	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

	
		//�洢���˱����еĴ���
		TMap<AEnemy*, uint32>HitMap;

		//����Ϊ��ʹ������λ�ó������е�Ŀ�꣬ȷ������ 
		//Ҫ�õ� End ��� ����λ�ã���Ҫ��������λ�� ���� Start ������꣬���ܵõ����յ� End �����ꡣ
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AEnemy* Enemy = Cast<AEnemy>(FireHit.GetActor());
			if (Enemy && InstigatorController)
			{
				//���Ӽ���: ��� DCharacter ������ HitMap �У�����Ὣ��Ӧ��ɫ�ļ����� 1��
				//����½�ɫ: ��� DCharacter �������� HitMap �У�����Ὣ DCharacter ��ӵ��ֵ��У������������ʼ��Ϊ 1��
				if (HitMap.Contains(Enemy))
				{
					HitMap[Enemy]++;
				}
				else
				{
					HitMap.Emplace(Enemy, 1);
				}
			}
			else 
			{
				AActor* Shooter = Cast<ADCharacter>(GetOwner());
				AActor* HitActor = FireHit.GetActor();
				// �ж��Ƿ�Ϊ
				if (HitActor->IsA(AExplosive::StaticClass()))
				{

					// ���Ե��� BulletHit_Implementation �ӿ�
					IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(HitActor);
						if (BulletHitInterFace)
						{
							BulletHitInterFace->BulletHit_Implementation(FireHit, Shooter, InstigatorController);
						}

				}
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this,
					HitSound, 
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-.5f,.5f)
				);
			}
			
		}
		for (auto HitPair : HitMap)
		{
			//key���Ƿ��صĽ�ɫ
				if (HitPair.Key && InstigatorController)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,//��ʩ���˺��Ľ�ɫ
						Damage*HitPair.Value,//ʩ���˺�����
						InstigatorController,//�ճ��˺��Ŀ���������
						this,//����˺��� Actor ���󣨵�ǰ����
						UDamageType::StaticClass()//�˺������ͣ�ָ��Ϊ UDamageType �ľ�̬��
					);
				}
		}
		
	}
	
}