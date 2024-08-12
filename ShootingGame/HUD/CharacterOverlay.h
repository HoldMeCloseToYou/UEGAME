// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	//ʹ��bindWidget����ͼ�����Ŀؼ�
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	//�ӵ��ı�
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoText;

	//�����ӵ�����
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponAmmoAmount;

	//Я�����ӵ�
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CarriedAmmoAmount;
};
