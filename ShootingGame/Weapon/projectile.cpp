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

//投射物（projectile）,即子弹
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
			//是要将粒子效果附加到的碰撞盒。
			CollisionBox,//要将粒子效果附加到的组件
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			//表示粒子效果应该保持在世界空间中的位置
			EAttachLocation::KeepWorldPosition

		);
	}
	//添加多播动态委托
	//（CollisionBox）的 OnComponentHit 事件动态绑定。每当碰撞盒与其他物体发生碰撞时，都会触发 OnHit。
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
		// 获取敌人击中声音
		EnemySound = Enemy->ImpactSound;

		Destroy();
	}
	else 
	{
		if(OtherActor->IsA(AExplosive::StaticClass()))
		{
			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			AController* InstigatorController = OwnerPawn->GetController();
			// 尝试调用 BulletHit_Implementation 接口
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
		//是 Unreal Engine 中用于在游戏世界中生成并附加Niagara系统的函数。它返回一个对生成的 UNiagaraComponent 的引用。
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,		//使用跟踪系统
			GetRootComponent(),	//附加的组件
			FName(),//可选的附加点名称，用于指定骨骼或者socket
			GetActorLocation(),//位置
			GetActorRotation(),//旋转
			EAttachLocation::KeepWorldPosition,//保持世界坐标，相对于附加点
			false//设置自动摧毁
		);
	}
}

void Aprojectile::ExplodeDamage()
{

	
	//if (Weapon)
	//{
	//	// 获取武器伤害
	//	
	//	DamageValue = Weapon->GetDamage();
	//}

	APawn* FiringPawn = GetInstigator();
	if (FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{

			//设置带有伤害衰减半径的
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,//因为projectile有定义伤害，这里可以直接使用，最大伤害
				20.f,//最小伤害
				GetActorLocation(),//伤害半径由来
				DamageInnerRadius,// 200内半径，
				DamageOuterRadius,// 500外半径
				5.f,	//伤害衰减
				UDamageType::StaticClass(),//伤害类
				TArray<AActor*>(),//这里传入了空指针数组，是希望所有人都会受到火箭弹伤害
				this,//造成伤害的物品，就是火箭弹
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
		&Aprojectile::DestroyTimerFinished,//完成后的回调函数
		DestroyTime//摧毁时间
	);
}
void Aprojectile::DestroyTimerFinished()
{
	Destroy();
}

//当actor生成的时候吗，这函数输入的任何东西都会被销毁
void Aprojectile::Destroyed()
{
	Super::Destroyed();
	
	//判断是否击中的是敌人
	if (bHitEnemy)
	{
		//if (EnemyParticles)
		//{
		//	//生成粒子效果
		//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyParticles, GetActorTransform());
		//}
		//if (EnemySound)
		//{
		//	//播放声音
		//	UGameplayStatics::PlaySoundAtLocation(this, EnemySound, GetActorLocation());
		//}
	}
	else
	{
		if (ImpactParticles)
		{
			//生成粒子效果
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
		}
		if (ImpactSound)
		{
			//播放声音
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
	}
}

