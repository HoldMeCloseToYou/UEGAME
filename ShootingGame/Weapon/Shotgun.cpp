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

	
		//存储敌人被击中的次数
		TMap<AEnemy*, uint32>HitMap;

		//乘是为了使得最终位置超过击中的目标，确保击中 
		//要得到 End 点的 绝对位置，需要将这个相对位置 加上 Start 点的坐标，才能得到最终的 End 点坐标。
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AEnemy* Enemy = Cast<AEnemy>(FireHit.GetActor());
			if (Enemy && InstigatorController)
			{
				//增加计数: 如果 DCharacter 存在于 HitMap 中，代码会将对应角色的计数加 1。
				//添加新角色: 如果 DCharacter 不存在于 HitMap 中，代码会将 DCharacter 添加到字典中，并将其计数初始化为 1。
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
				// 判断是否为
				if (HitActor->IsA(AExplosive::StaticClass()))
				{

					// 尝试调用 BulletHit_Implementation 接口
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
			//key就是返回的角色
				if (HitPair.Key && InstigatorController)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,//被施加伤害的角色
						Damage*HitPair.Value,//施加伤害的量
						InstigatorController,//照成伤害的控制器对象
						this,//造成伤害的 Actor 对象（当前对象）
						UDamageType::StaticClass()//伤害的类型，指定为 UDamageType 的静态类
					);
				}
		}
		
	}
	
}