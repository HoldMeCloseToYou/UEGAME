// Fill out your copyright notice in the Description page of Project Settings.


#include "projectile.h"
#include"Components/BoxComponent.h"
#include"GameFramework/ProjectileMovementComponent.h"
#include"Kismet/GameplayStatics.h"
#include"Particles/ParticleSystemComponent.h"
#include"Particles/ParticleSystem.h"
#include"Sound/SoundCue.h"
#include"ShootingGame/Character/Enemy.h"
#include"ShootingGame/Explosive/Explosive.h"
#include"NiagaraFunctionLibrary.h"
#include"RocketMovementComponent.h"

//Ͷ���projectile��,���ӵ�
Aprojectile::Aprojectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility,ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

void Aprojectile::BeginPlay()
{
	Super::BeginPlay();

	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			//��Ҫ������Ч�����ӵ�����ײ�С�
			CollisionBox,//Ҫ������Ч�����ӵ������
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			//��ʾ����Ч��Ӧ�ñ���������ռ��е�λ��
			EAttachLocation::KeepWorldPosition

		);
	}
	//��Ӷಥ��̬ί��
	//��CollisionBox���� OnComponentHit �¼���̬�󶨡�ÿ����ײ�����������巢����ײʱ�����ᴥ�� OnHit��
	CollisionBox->OnComponentHit.AddDynamic(this, &Aprojectile::OnHit);
	
}



void Aprojectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA<APawn>())
	{
		bHitEnemy = true;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit Enemy!"));
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy == nullptr)return;
		EnemyParticles = Enemy->ImpactParticles;
		// ��ȡ���˻�������
		EnemySound = Enemy->ImpactSound;

		Destroy();
	}
	else 
	{
		if(OtherActor->IsA(AExplosive::StaticClass()))
		{
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			AController* InstigatorController = OwnerPawn->GetController();
			// ���Ե��� BulletHit_Implementation �ӿ�
			IBulletHitInterface* BulletHitInterFace = Cast<IBulletHitInterface>(OtherActor);
			if (BulletHitInterFace)
			{
				BulletHitInterFace->BulletHit_Implementation(Hit ,GetOwner(), InstigatorController);
			}
			
		}
		else
		{
			bHitEnemy = false;
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("NoHit Enemy!"));
			Destroy();
		}
		
	}
}

void Aprojectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		//�� Unreal Engine ����������Ϸ���������ɲ�����Niagaraϵͳ�ĺ�����������һ�������ɵ� UNiagaraComponent �����á�
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,		//ʹ�ø���ϵͳ
			GetRootComponent(),	//���ӵ����
			FName(),//��ѡ�ĸ��ӵ����ƣ�����ָ����������socket
			GetActorLocation(),//λ��
			GetActorRotation(),//��ת
			EAttachLocation::KeepWorldPosition,//�����������꣬����ڸ��ӵ�
			false//�����Զ��ݻ�
		);
	}
}

void Aprojectile::ExplodeDamage()
{

	
	//if (Weapon)
	//{
	//	// ��ȡ�����˺�
	//	
	//	DamageValue = Weapon->GetDamage();
	//}

	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{

			//���ô����˺�˥���뾶��
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,//��Ϊprojectile�ж����˺����������ֱ��ʹ�ã�����˺�
				20.f,//��С�˺�
				GetActorLocation(),//�˺��뾶����
				DamageInnerRadius,// 200�ڰ뾶��
				DamageOuterRadius,// 500��뾶
				5.f,	//�˺�˥��
				UDamageType::StaticClass(),//�˺���
				TArray<AActor*>(),//���ﴫ���˿�ָ�����飬��ϣ�������˶����ܵ�������˺�
				this,//����˺�����Ʒ�����ǻ����
				FiringController
			);
		}
	}
}

void Aprojectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void Aprojectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&Aprojectile::DestroyTimerFinished,//��ɺ�Ļص�����
		DestroyTime//�ݻ�ʱ��
	);
}
void Aprojectile::DestroyTimerFinished()
{
	Destroy();
}

//��actor���ɵ�ʱ�����⺯��������κζ������ᱻ����
void Aprojectile::Destroyed()
{
	Super::Destroyed();
	
	//�ж��Ƿ���е��ǵ���
	if (bHitEnemy)
	{
		//if (EnemyParticles)
		//{
		//	//��������Ч��
		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyParticles, GetActorTransform());
		//}
		//if (EnemySound)
		//{
		//	//��������
		//	UGameplayStatics::PlaySoundAtLocation(this, EnemySound, GetActorLocation());
		//}
	}
	else
	{
		if (ImpactParticles)
		{
			//��������Ч��
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
		if (ImpactSound)
		{
			//��������
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
	}
}

