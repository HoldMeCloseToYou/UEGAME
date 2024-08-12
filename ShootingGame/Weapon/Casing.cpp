// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include "TimerManager.h"
// Sets default values
ACasing::ACasing()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	//将CasingMesh与摄像机通道（ECC_Camera）的碰撞响应设置为忽略（ECR_Ignore）
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//设置模拟物理和开启重力
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	//设置初速度
	ShellEjectionImpuse = 5.f;

}


void ACasing::BeginPlay()
{
	Super::BeginPlay();
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector()*ShellEjectionImpuse);
}


void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound,GetActorLocation());
	}
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ACasing::Destroyed, 2.0f, false);
	Destroyed();
}


void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

