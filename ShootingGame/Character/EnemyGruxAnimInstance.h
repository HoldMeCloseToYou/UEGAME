// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyGruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API UEnemyGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimtionProperties(float DeltaTime);

private:
	
	//移动速度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;


	//对敌人的引用
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemy* Enemy;

};
