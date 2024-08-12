// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget)override;


private:

	UPROPERTY(EditAnywhere,Category="Weapon Scatter")
	//���Ҫ�洢һ����ҵĵ÷֣�����ʹ�� uint32����Ϊ�÷ֲ�����Ϊ������
	//���Ҫ�洢һ����������꣬����ʹ�� int32����Ϊ�������Ϊ���������������
	uint32 NumberOfPellets = 12;
	
};
