// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include"Kismet/GameplayStatics.h"
#include"Sound/SoundCue.h"
#include"Particles/ParticleSystemComponent.h"
#include"Blueprint/UserWidget.h"
#include"Kismet/KismetMathLibrary.h"
#include"DrawDebugHelpers.h"
#include"EnemyAIController.h"
#include"BehaviorTree/BehaviorTreeComponent.h"
#include"BehaviorTree/BlackboardComponent.h"
#include"Components/SphereComponent.h"
#include"DCharacter.h"
#include"Components/CapsuleComponent.h"
#include"Components/BoxComponent.h"
#include"ShootingGame/Combat/CombatComponent.h"
#include"Engine/SkeletalMeshSocket.h"

// Sets default values
AEnemy::AEnemy():
	Health(100.f),
	MaxHealth(100.f),
	HealthBarDisplayTime(4.f),
	bCanHitReact(true),
	HitReactTimeMin(.5f),
	HitReactTimeMax(.75f),
	HitNumberDestroyTime(1.5f),
	bStunned(false),
	StunChance(0.5f),
	AttackLFast(TEXT("AttackLFast")),
	AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")),
	AttackR(TEXT("AttackR")),
	BaseDamage(20.f),
	LeftWeaponSocket(TEXT("FX_Trail_L_01")),
	RightWeaponSocket(TEXT("FX_Trail_R_01")),
	bCanAttack(true),
	AttackWaitTime(1.f),
	bDying(false),
	DeathTime(4.f)

{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//创建agro球体
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());


	//攻击范围球体
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());


	//左右武器碰撞盒子
	LeftWeaponCollision= CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	RightWeaponCollision= CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//绑定函数
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);


	//绑定武器碰撞盒子函数
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);


	//设置敌人的碰撞，通道为:可视并且反应为:阻挡
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//忽略Mesh和胶囊体
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);


	//设置武器开始不触发碰撞，在正确的时间才使用
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//只和pawn发生交互
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//只和pawn发生交互
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//
	EnemyController = Cast<AEnemyAIController>(GetController());

	//设置敌人可以攻击的默认值
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);

	}
	//根据当前Actor的变换信息，转换为世界坐标系下的位置
	const FVector WoldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);

	const FVector WoldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	/*DrawDebugSphere(
		GetWorld(),
		WoldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true
	);


	DrawDebugSphere(
		GetWorld(),
		WoldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true
	);*/

	//设置黑板上的键值
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WoldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WoldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
	

}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);//这行代码用于清除之前设置的 HealthBarTimer。
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,//用于标识要设置的计时器
		this,
		&AEnemy::HidHealthBar,//该函数将在计时器结束时被调用
		HealthBarDisplayTime//表示计时器持续的时间
	);
}

void AEnemy::Die()
{
	if (bDying)return;
	bDying = true;
	HidHealthBar();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		EnemyController->StopMovement();
	}

}

void AEnemy::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	GetWorldTimerManager().SetTimer(
		DeathTimer,
		this,
		&AEnemy::DestroyEnemy,
		DeathTime
	);
}

void AEnemy::DestroyEnemy()
{
	Destroy();
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);

		}
	}
	else
	{
		bCanHitReact = false;
		const float HitReactTime{ FMath::FRandRange(HitReactTimeMin,HitReactTimeMax) };
		GetWorldTimerManager().SetTimer(
			HitReactTimer,
			this,
			&AEnemy::ResetHitReactTimer,
			HitReactTime
		);
	}
	
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{

	HitNumbers.Add(HitNumber, Location);

	//FTimerHandle 用于管理定时器，而 FTimerDelegate 用于绑定到定时器触发时要调用的函数。
	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	//this 表示当前对象实例，FName("DestroyHitNumber") 表示要绑定的函数名，而 HitNumber 则是作为参数传递给绑定的函数的值。
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"),HitNumber);

	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer,//
		HitNumberDelegate,//触发时候要调用的对象实例
		HitNumberDestroyTime,//是一个 float 类型的参数，表示定时器触发的时间间隔（以秒为单位）
		false//是否循环
	);
}

void AEnemy::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();

}

void AEnemy::UpdateHitNumbers()
{

	if (bDying)return;
	for (auto& HitPair : HitNumbers)
	{
		//获取TMap里面的值
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };

		FVector2D ScreenPosition;
		//可以将给定的世界坐标转换为相对于玩家视角的屏幕坐标
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition
		);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;
	auto Character = Cast<ADCharacter>(OtherActor);
	if (Character)
	{
		if (EnemyController)
		{
			if (EnemyController->GetBlackboardComponent())
			{
				//设置键值
				EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
			}
		}
		
	}
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;

	if (EnemyController)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Stunned"));
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr)return;
	auto ShootCharacter = Cast<ADCharacter>(OtherActor);
	if(ShootCharacter)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
			
		}
	}
	

}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (OtherActor == nullptr)return;
	auto ShootCharacter = Cast<ADCharacter>(OtherActor);
	if (ShootCharacter)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	if (bInAttackRange)
	{
		UAnimInstance* AttackAnimInstance = GetMesh()->GetAnimInstance();
		if (AttackAnimInstance)
		{
			AttackAnimInstance->Montage_Play(AttackMontage);
			AttackAnimInstance->Montage_JumpToSection(Section, AttackMontage);

		}
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(
			AttackWaitTimer,
			this,
			&AEnemy::ResetCanAttack,
			AttackWaitTime
		);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);

		}
	}
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(1,4) };
	switch (Section)
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}
	return SectionName;
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<ADCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character,LeftWeaponSocket);
		StunCharacter(Character);
	}
	
	

}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<ADCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character, RightWeaponSocket);
		StunCharacter(Character);
	}

}

void AEnemy::SpawnBlood(ADCharacter* Victim, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if (TipSocket)
	{
		// GetMesh()通过传入这个网格体，GetSocketTransform 就可以确定插槽在该网格体上的变换信息
		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetBloodParticles())
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				Victim->GetBloodParticles(),
				SocketTransform
			);
		}
	}
}

void AEnemy::StunCharacter(ADCharacter* Victim)
{
	if (Victim)
	{
		const float Stun{ FMath::FRandRange(0.f,1.f) };
		if (Stun <= Victim->StunChance)
		{
			Victim->Combat->Stun();
		}
	}
}

void AEnemy::ResetCanAttack()
{
	bCanAttack = true;

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);

	}

}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage(ADCharacter* Victim)
{
	if (Victim == nullptr)return;
	if (Victim)
	{
		UGameplayStatics::ApplyDamage(
			Victim,//谁受到伤害
			BaseDamage,
			EnemyController,
			this,
			UDamageType::StaticClass()
		);
		if (Victim->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				Victim->GetMeleeImpactSound(),
				GetActorLocation()
				);
		}
		if (Victim->GetBloodParticles())
		{
		
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterContorller)
{
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ImpactParticles,
			HitResult.ImpactPoint,
			FRotator(0.f),
			true
		);
	}

	
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//设置黑板值，使得射击敌人，可以获得射击者（仇恨）
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			FName("Target"),
			DamageCauser->GetOwner());
	}
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
	{
		Health -= DamageAmount;
	}

	if (bDying)return DamageAmount;

	//展示血量条
	ShowHealthBar();

	//击中停顿
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{

		//播放受击动画
		//if(HitResult.BoneName.ToString()==)
		PlayHitMontage(FName("HitReactFront"), 1.0f);
		SetStunned(true);
	}
	return DamageAmount;
}
