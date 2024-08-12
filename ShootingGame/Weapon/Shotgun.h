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
	//如果要存储一个玩家的得分，可以使用 uint32，因为得分不可能为负数。
	//如果要存储一个物体的坐标，可以使用 int32，因为坐标可能为负数、零或正数。
	uint32 NumberOfPellets = 12;
	
};
