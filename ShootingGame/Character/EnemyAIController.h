// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()


public:
	AEnemyAIController();
	//在AI角色被控制时进行初始化和设置操作，以确保AI角色在被控制之后能够正确地执行所需的行为。
	virtual void OnPossess(APawn* InPawn)override;



private:

	//ai 黑板
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;
	

	//AI行为树
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;


public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent()const { return BlackboardComponent; }
};
