// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
//����Ҫ���ڴ����������Skeletal Mesh���еĲ�ۣ�Socket)
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
		//����Ϊ��ʹ������λ�ó������е�Ŀ�꣬ȷ������
		//Ҫ�õ� End ��� ����λ�ã���Ҫ��������λ�� ���� Start ������꣬���ܵõ����յ� End �����ꡣ
		FHitResult FireHit;

		//����
		WeaponTraceHit(Start, HitTarget, FireHit);
		AActor* Shooter = Cast<ADCharacter>(GetOwner());

			// ��ȡ����Ŀ��� Actor
			AActor* HitActor = FireHit.GetActor();
			// �ж��Ƿ�Ϊ����
			if (HitActor->IsA(AEnemy::StaticClass()))
			{
				// ���е��ˣ�ִ������߼�
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit Enemy!"));

				// ���Ե��� BulletHit_Implementation �ӿ�
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
						//��ͷ
						DamageNum = HeadShotDamage;
						UGameplayStatics::ApplyDamage(
						HitActor,//��ʩ���˺��Ľ�ɫ
						DamageNum,//ʩ���˺�����
						InstigatorController,//�ճ��˺��Ŀ���������
						this,//����˺��� Actor ���󣨵�ǰ����
						UDamageType::StaticClass()//�˺������ͣ�ָ��Ϊ UDamageType �ľ�̬��
						);

						if (HitEnemy->bDying)return;
						HitEnemy->ShowHitNumber(DamageNum, FireHit.Location,true);
					}
					else
					{
						//��������
						DamageNum = Damage;
						UGameplayStatics::ApplyDamage(
							HitActor,//��ʩ���˺��Ľ�ɫ
							DamageNum,//ʩ���˺�����
							InstigatorController,//�ճ��˺��Ŀ���������
							this,//����˺��� Actor ���󣨵�ǰ����
							UDamageType::StaticClass()//�˺������ͣ�ָ��Ϊ UDamageType �ľ�̬��
						);

						if (HitEnemy->bDying)return;
						HitEnemy->ShowHitNumber(DamageNum,FireHit.Location,false);
					}
					
					
				}
				//������������Ч
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
				//ǹ������
				if (MuzzleFlash)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						MuzzleFlash,
						SocketTransform
					);
				}
				//��������
				if (FireSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
				}
			}
			else
			{
				if (HitActor->IsA(AExplosive::StaticClass()))
				{
					// ���Ե��� BulletHit_Implementation �ӿ�
					IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(HitActor);
					if (BulletHitInterFace)
					{
						BulletHitInterFace->BulletHit_Implementation(FireHit,Shooter, InstigatorController);
					}
				}
				else
				{
					// ���зǵ��ˣ�ִ�������߼�
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Hit Non-Enemy!"));
					//������������Ч
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
			//ǹ������
					if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					MuzzleFlash,
					SocketTransform
				);
			}
			//��������
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
	//GetSafeNormal()���ú������ڽ�һ��������һ����ʹ�䳤��Ϊ 1,��������˷��䵽���еķ���
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	//�ӿ�ʼ�����е�ĳ��λ��,�������ģ�ǹ��λ��+������*����ľ��룩
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	//�����������㣬ʹ��������ɵķ���ĵ�λ����* 0�������С����������õ�һ���䵯λ��
	FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	//��������ļ������λ�õõ�endloc
	FVector EndLoc = SphereCenter + RandVec;
	//ToEndLoc�õ����Ŀ�������
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

	//�������ߵ��յ㣬ǹ�����+������λ��*�̶��ĳ���/���ߵĳ��ȴ�С
	return FVector(TraceStart+ToEndLoc*TRACE_LENGTH/ToEndLoc.Size());
}

// ����ģ��������ɨ�����������߼����Ӿ�Ч�������������߼�������ɹ���Ч��
void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart,const FVector& HitTarget,FHitResult& OutHit)
{
	FHitResult FireHit;
	UWorld* World = GetWorld();
	if (World)
	{
		//�ж��Ƿ�ʹ����ɢ
		FVector End = bUseScatter? TraceEndWithScatter(TraceStart,HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		//��·����
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
		//�������
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
