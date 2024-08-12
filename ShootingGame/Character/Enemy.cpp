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


	//����agro����
	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());


	//������Χ����
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());


	//����������ײ����
	LeftWeaponCollision= CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponCollision"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	RightWeaponCollision= CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponCollision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//�󶨺���
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);


	//��������ײ���Ӻ���
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);


	//���õ��˵���ײ��ͨ��Ϊ:���Ӳ��ҷ�ӦΪ:�赲
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//����Mesh�ͽ�����
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);


	//����������ʼ��������ײ������ȷ��ʱ���ʹ��
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//ֻ��pawn��������
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//ֻ��pawn��������
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//
	EnemyController = Cast<AEnemyAIController>(GetController());

	//���õ��˿��Թ�����Ĭ��ֵ
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);

	}
	//���ݵ�ǰActor�ı任��Ϣ��ת��Ϊ��������ϵ�µ�λ��
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

	//���úڰ��ϵļ�ֵ
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WoldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WoldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
	

}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);//���д����������֮ǰ���õ� HealthBarTimer��
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,//���ڱ�ʶҪ���õļ�ʱ��
		this,
		&AEnemy::HidHealthBar,//�ú������ڼ�ʱ������ʱ������
		HealthBarDisplayTime//��ʾ��ʱ��������ʱ��
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

	//FTimerHandle ���ڹ���ʱ������ FTimerDelegate ���ڰ󶨵���ʱ������ʱҪ���õĺ�����
	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;

	//this ��ʾ��ǰ����ʵ����FName("DestroyHitNumber") ��ʾҪ�󶨵ĺ��������� HitNumber ������Ϊ�������ݸ��󶨵ĺ�����ֵ��
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"),HitNumber);

	GetWorld()->GetTimerManager().SetTimer(
		HitNumberTimer,//
		HitNumberDelegate,//����ʱ��Ҫ���õĶ���ʵ��
		HitNumberDestroyTime,//��һ�� float ���͵Ĳ�������ʾ��ʱ��������ʱ����������Ϊ��λ��
		false//�Ƿ�ѭ��
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
		//��ȡTMap�����ֵ
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };

		FVector2D ScreenPosition;
		//���Խ���������������ת��Ϊ���������ӽǵ���Ļ����
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
				//���ü�ֵ
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
		// GetMesh()ͨ��������������壬GetSocketTransform �Ϳ���ȷ������ڸ��������ϵı任��Ϣ
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
			Victim,//˭�ܵ��˺�
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
	//���úڰ�ֵ��ʹ��������ˣ����Ի������ߣ���ޣ�
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

	//չʾѪ����
	ShowHealthBar();

	//����ͣ��
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{

		//�����ܻ�����
		//if(HitResult.BoneName.ToString()==)
		PlayHitMontage(FName("HitReactFront"), 1.0f);
		SetStunned(true);
	}
	return DamageAmount;
}
