#pragma once


UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),//���е�
	ECS_Reloading UMETA(DisplayName="Reloading"),//����״̬
	ECS_Stunned UMETA(DisplayName="Stunned"),//����״̬
	ECS_MAX UMETA(DisplayName="DefaultMax")

};