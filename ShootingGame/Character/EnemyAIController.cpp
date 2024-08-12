// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include"BehaviorTree/BlackboardComponent.h"
#include"BehaviorTree/BehaviorTreeComponent.h"
#include"BehaviorTree/BehaviorTree.h"
#include"Enemy.h"



AEnemyAIController::AEnemyAIController()
{
	//给初始化，创建一些默认的对象
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);

}
void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn == nullptr)return;
	
	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			//获取了Enemy对象关联的行为树的黑板资产
			//使用*操作符是因为InitializeBlackboard函数接受的是一个指向黑板资产的指针，
			//而获取到的黑板资产本身可能是一个指针类型。通过使用*操作符，可以将指针解引用，
			//获取到实际的黑板资产对象，然后将其传递给InitializeBlackboard函数。
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}
	
}