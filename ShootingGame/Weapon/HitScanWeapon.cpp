// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
//类主要用于处理骨骼网格（Skeletal Mesh）中的插槽（Socket)
#include"ShootingGame/Character/DCharacter.h"
#include"Engine/SkeletalMeshSocket.h"
#include"Kismet/GamePlayStatics.h"
#include"Kismet/KismetMathLibrary.h"
#include"NiagaraFunctionLibrary.h"
#include"NiagaraComponent.h"
#include"Sound/SoundCue.h"
#include"Particles/ParticleSystemComponent.h"
#include"WeaponTypes.h"
#include"ShootingGame/Character/BulletHitInterface.h"
#include"ShootingGame/Character/Enemy.h"
#include"ShootingGame/Explosive/Explosive.h"
#include"DrawDebugHelpers.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)return;

	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		//乘是为了使得最终位置超过击中的目标，确保击中
		//要得到 End 点的 绝对位置，需要将这个相对位置 加上 Start 点的坐标，才能得到最终的 End 点坐标。
		FHitResult FireHit;

		//武器
		WeaponTraceHit(Start, HitTarget, FireHit);
		AActor* Shooter = Cast<ADCharacter>(GetOwner());

			// 获取击中目标的 Actor
			AActor* HitActor = FireHit.GetActor();
			// 判断是否为敌人
			if (HitActor->IsA(AEnemy::StaticClass()))
			{
				// 击中敌人，执行相关逻辑
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit Enemy!"));

				// 尝试调用 BulletHit_Implementation 接口
				IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(HitActor);
				if (BulletHitInterFace)
				{
					BulletHitInterFace->BulletHit_Implementation(FireHit, Shooter, InstigatorController);
				}
				AEnemy* HitEnemy = Cast<AEnemy>(FireHit.GetActor());
				if (InstigatorController)
				{
					int32 DamageNum{};
					if (FireHit.BoneName.ToString() == HitEnemy->GetHeadBone())
					{
						//爆头
						DamageNum = HeadShotDamage;
						UGameplayStatics::ApplyDamage(
						HitActor,//被施加伤害的角色
						DamageNum,//施加伤害的量
						InstigatorController,//照成伤害的控制器对象
						this,//造成伤害的 Actor 对象（当前对象）
						UDamageType::StaticClass()//伤害的类型，指定为 UDamageType 的静态类
						);

						if (HitEnemy->bDying)return;
						HitEnemy->ShowHitNumber(DamageNum, FireHit.Location,true);
					}
					else
					{
						//击中身体
						DamageNum = Damage;
						UGameplayStatics::ApplyDamage(
							HitActor,//被施加伤害的角色
							DamageNum,//施加伤害的量
							InstigatorController,//照成伤害的控制器对象
							this,//造成伤害的 Actor 对象（当前对象）
							UDamageType::StaticClass()//伤害的类型，指定为 UDamageType 的静态类
						);

						if (HitEnemy->bDying)return;
						HitEnemy->ShowHitNumber(DamageNum,FireHit.Location,false);
					}
					
					
				}
				//如果冲击粒子有效
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
					UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
				}
				//枪口闪光
				if (MuzzleFlash)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						MuzzleFlash,
						SocketTransform
					);
				}
				//开火声音
				if (FireSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
				}
			}
			else
			{
				if (HitActor->IsA(AExplosive::StaticClass()))
				{
					// 尝试调用 BulletHit_Implementation 接口
					IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(HitActor);
					if (BulletHitInterFace)
					{
						BulletHitInterFace->BulletHit_Implementation(FireHit,Shooter, InstigatorController);
					}
				}
				else
				{
					// 击中非敌人，执行其他逻辑
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Hit Non-Enemy!"));
					//如果冲击粒子有效
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
						UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
					}
			//枪口闪光
					if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					MuzzleFlash,
					SocketTransform
				);
			}
			//开火声音
					if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
					}
			}

			

	}
}
	
FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	//GetSafeNormal()：该函数用于将一个向量归一化，使其长度为 1,这里计算了发射到击中的方向
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	//从开始到击中的某处位置,球体中心，枪口位置+（方向*射击的距离）
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	//这是是随机射点，使用随机生成的方向的单位向量* 0到球体大小的随机数，得到一个射弹位置
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	//在射击中心加上随机位置得到endloc
	FVector EndLoc = SphereCenter + RandVec;
	//ToEndLoc得到射击目标的射线
	FVector ToEndLoc = EndLoc - TraceStart;
	/*DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc,4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Cyan,
		true
	);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		EndLoc,
		FColor::Red,
		true
	);*/

	//返回射线的终点，枪口起点+结束的位置*固定的长度/射线的长度大小
	return FVector(TraceStart+ToEndLoc*TRACE_LENGTH/ToEndLoc.Size());
}

// 函数模拟了命中扫描武器的射线检测和视觉效果，它根据射线检测结果生成光束效果
void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart,const FVector& HitTarget,FHitResult& OutHit)
{
	FHitResult FireHit;
	UWorld* World = GetWorld();
	if (World)
	{
		//判断是否使用扩散
		FVector End = bUseScatter? TraceEndWithScatter(TraceStart,HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		//线路跟踪
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
			);

		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
			
		}
		//射击烟雾
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}
