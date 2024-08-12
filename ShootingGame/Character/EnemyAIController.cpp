// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include"BehaviorTree/BlackboardComponent.h"
#include"BehaviorTree/BehaviorTreeComponent.h"
#include"BehaviorTree/BehaviorTree.h"
#include"Enemy.h"



AEnemyAIController::AEnemyAIController()
{
	//����ʼ��������һЩĬ�ϵĶ���
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
			//��ȡ��Enemy�����������Ϊ���ĺڰ��ʲ�
			//ʹ��*����������ΪInitializeBlackboard�������ܵ���һ��ָ��ڰ��ʲ���ָ�룬
			//����ȡ���ĺڰ��ʲ����������һ��ָ�����͡�ͨ��ʹ��*�����������Խ�ָ������ã�
			//��ȡ��ʵ�ʵĺڰ��ʲ�����Ȼ���䴫�ݸ�InitializeBlackboard������
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}
	
}