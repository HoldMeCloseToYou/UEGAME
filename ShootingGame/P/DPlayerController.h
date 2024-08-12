// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API ADPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDWeaponAmmo(int Ammo);
	void SetHUDCarriedAmmo(int Ammo);
protected:
	virtual void BeginPlay()override;

private:
	class ADHUD* DHUD;
};
