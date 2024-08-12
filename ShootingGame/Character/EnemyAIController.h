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
	//��AI��ɫ������ʱ���г�ʼ�������ò�������ȷ��AI��ɫ�ڱ�����֮���ܹ���ȷ��ִ���������Ϊ��
	virtual void OnPossess(APawn* InPawn)override;



private:

	//ai �ڰ�
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;
	

	//AI��Ϊ��
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior",meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;


public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent()const { return BlackboardComponent; }
};
