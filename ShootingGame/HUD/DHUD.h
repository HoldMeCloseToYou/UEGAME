// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DHUD.generated.h"


//蓝图结构体,它被设计用来存储HUD元素的相关信息
USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;

	//添加十字准线浮点数，使得动态
	float CrosshairSpread;
};
/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API ADHUD : public AHUD
{
	GENERATED_BODY()
	
public :
	virtual void DrawHUD() override;

	//TSubclassOf<class UUserWidget>: 这是一个模板类，
	//它声明了一个指向 UUserWidget 类,CharacterOverlayClass。
	UPROPERTY(EditAnywhere,Category="Player Stats")
	TSubclassOf<class UUserWidget>CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();



private:
	FHUDPackage HUDPackage;

	//第一个参数为纹理，第二为游戏窗口中心
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
