// Fill out your copyright notice in the Description page of Project Settings.


#include "projectileRocket.h"
#include"Kismet/GameplayStatics.h"
#include"NiagaraFunctionLibrary.h"
#include"NiagaraComponent.h"
#include"Components/BoxComponent.h"
#include"Sound/SoundCue.h"
#include"Weapon.h"
#include"Components/AudioComponent.h"

AprojectileRocket::AprojectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));

	RocketMovementComponent->bRotationFollowsVelocity = true;
}

void AprojectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AprojectileRocket::OnHit);
	}

	SpawnTrailSystem();

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),//获取根组件
			FName(),
			GetActorLocation(),//
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,//体积
			1.f,//声音增强
			0.f,//音频开始时间
			LoopingSoundAttenuation,//循环声音衰减
			(USoundConcurrency*)nullptr,
			false
		);
	}


}



void AprojectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//获取武器拥有者
	if (OtherActor == GetOwner())
	{
		return;
	}
	AWeapon* Weapon = Cast<AWeapon>(GetOwner());
	ExplodeDamage();

	//设定计时器延迟3秒
	StartDestroyTimer();


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

	//设置火箭模型的可视性
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	//设置碰撞
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


	//获取存储在组件中的Niagara的系统实例，当onhit的时候停止系统实例的运行
	if (TrailSystemComponent &&TrailSystemComponent->GetSystemInstance())
	{
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}

	if (ProjectileLoopComponent&&ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}

void AprojectileRocket::Destroyed()
{
	

}
