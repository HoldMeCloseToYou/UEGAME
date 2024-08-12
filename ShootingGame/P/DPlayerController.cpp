// Fill out your copyright notice in the Description page of Project Settings.


#include "DPlayerController.h"
#include"ShootingGame/HUD/DHUD.h"
#include"ShootingGame/HUD/CharacterOverlay.h"
//����Ѫ���������������
#include"Components/ProgressBar.h"
#include"Components/TextBlock.h"



void ADPlayerController::BeginPlay()
{
	Super::BeginPlay();

	DHUD = Cast<ADHUD>(GetHUD());
}

void ADPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	DHUD = DHUD == nullptr ? Cast<ADHUD>(GetHUD()) : DHUD;

	bool bHUDValid = DHUD &&
		DHUD->CharacterOverlay &&
		DHUD->CharacterOverlay->HealthBar &&
		DHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		//����Ѫ���ٷ���
		const float HealthPercent = Health / MaxHealth;
		//����Ѫ���İٷ�ֵ
		DHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);

		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		DHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}

}

void ADPlayerController::SetHUDWeaponAmmo(int Ammo)
{
	DHUD = DHUD == nullptr ? Cast<ADHUD>(GetHUD()) : DHUD;
	bool bHUDValid = DHUD &&
		DHUD->CharacterOverlay &&
		DHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{


		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		DHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ADPlayerController::SetHUDCarriedAmmo(int Ammo)
{
	DHUD = DHUD == nullptr ? Cast<ADHUD>(GetHUD()) : DHUD;
	bool bHUDValid = DHUD &&
		DHUD->CharacterOverlay &&
		DHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{


		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		DHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}

}
