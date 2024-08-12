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
			GetRootComponent(),//��ȡ�����
			FName(),
			GetActorLocation(),//
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,//���
			1.f,//������ǿ
			0.f,//��Ƶ��ʼʱ��
			LoopingSoundAttenuation,//ѭ������˥��
			(USoundConcurrency*)nullptr,
			false
		);
	}


}



void AprojectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//��ȡ����ӵ����
	if (OtherActor == GetOwner())
	{
		return;
	}
	AWeapon* Weapon = Cast<AWeapon>(GetOwner());
	ExplodeDamage();

	//�趨��ʱ���ӳ�3��
	StartDestroyTimer();


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

	//���û��ģ�͵Ŀ�����
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	//������ײ
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


	//��ȡ�洢������е�Niagara��ϵͳʵ������onhit��ʱ��ֹͣϵͳʵ��������
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
