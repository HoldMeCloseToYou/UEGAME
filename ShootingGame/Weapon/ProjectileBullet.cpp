// Fill out your copyright notice in the Description page of Project Settings


#include "ProjectileBullet.h"
#include"kismet/GameplayStatics.h"
#include"GameFramework/Character.h"
#include"GameFramework/ProjectileMovementComponent.h"
#include"ShootingGame/Explosive/Explosive.h"
#include"ShootingGame/Character/Enemy.h"
#include"ShootingGame/Character/DCharacter.h"
#include"Weapon.h"



AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		//AWeapon* Weapon = Cast<AWeapon>(GetOwner());
		
		//if (Weapon)
		//{
		//	// 获取武器伤害
		//	DamageValue = Weapon->GetDamage();
		// 
		//}
		
		
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor());
			if (HitEnemy)
			{
				// 尝试调用 BulletHit_Implementation 接口
				IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(OtherActor);
				if (BulletHitInterFace)
				{
					AActor* Shooter = Cast<ADCharacter>(GetOwner());
					BulletHitInterFace->BulletHit_Implementation(Hit,Shooter,OwnerController);
				}

				float DamageValue = 20.f;
				float HeadShotDamageValue = 50.f;
				float DamageNum;
				if (Hit.BoneName.ToString() == HitEnemy->GetHeadBone())
				{
					
					DamageNum = HeadShotDamageValue;
					//击中头部
					UGameplayStatics::ApplyDamage(
						OtherActor,
						DamageNum,
						OwnerController,
						this,
						UDamageType::StaticClass()
					);
					if (HitEnemy->bDying)return;
					HitEnemy->ShowHitNumber(DamageNum, Hit.Location, true);
				}
				else
				{
					DamageNum = DamageValue;
					//击中身体
					UGameplayStatics::ApplyDamage(
						OtherActor,
						DamageNum,
						OwnerController,
						this,
						UDamageType::StaticClass()
					);

					if (HitEnemy->bDying)return;
					HitEnemy->ShowHitNumber(DamageNum,Hit.Location,false);
				}
				
			}
			
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

}