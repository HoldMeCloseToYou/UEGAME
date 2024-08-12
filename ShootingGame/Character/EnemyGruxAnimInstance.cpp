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
		//获得敌人的速度
		FVector Velocity{ Enemy->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
