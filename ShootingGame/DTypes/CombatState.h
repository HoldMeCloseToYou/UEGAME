#pragma once


UENUM(BlueprintType)
enum class ECombatState :uint8
{
	ECS_Unoccupied UMETA(DisplayName="Unoccupied"),//¿ÕÏÐµÄ
	ECS_Reloading UMETA(DisplayName="Reloading"),//»»µ¯×´Ì¬
	ECS_Stunned UMETA(DisplayName="Stunned"),//»÷ÖÐ×´Ì¬
	ECS_MAX UMETA(DisplayName="DefaultMax")

};