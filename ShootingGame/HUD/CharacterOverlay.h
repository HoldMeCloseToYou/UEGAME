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
	//使用bindWidget绑定蓝图创建的控件
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	//子弹文本
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoText;

	//武器子弹数量
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WeaponAmmoAmount;

	//携带的子弹
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CarriedAmmoAmount;
};
