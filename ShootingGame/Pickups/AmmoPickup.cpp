// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include"ShootingGame/Character/DCharacter.h"
#include"ShootingGame/Combat/CombatComponent.h"


void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OhterBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OhterBodyIndex, bFromSweep, SweepResult);
	ADCharacter* DCharacter = Cast<ADCharacter>(OtherActor);
	if (DCharacter)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("DCharacter is ok "));
		UCombatComponent* Combat = DCharacter->Combat;
		if (Combat)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Combat is ok "));
			Combat->PickupAmmo(WeaponType, AmmoAmount);
		}
	}

	Destroy();
}
