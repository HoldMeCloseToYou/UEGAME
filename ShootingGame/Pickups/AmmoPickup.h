// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include"ShootingGame/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
	

protected:
	virtual void OnSphereOverlap(
		//ÖØµþ
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OhterBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);


private:
	UPROPERTY(EditAnywhere)
		int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;

};
