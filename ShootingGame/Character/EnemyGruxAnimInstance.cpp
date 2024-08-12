// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGruxAnimInstance.h"
#include"Enemy.h"

void UEnemyGruxAnimInstance::UpdateAnimtionProperties(float DeltaTime)
{
	if (Enemy==nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		//��õ��˵��ٶ�
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
