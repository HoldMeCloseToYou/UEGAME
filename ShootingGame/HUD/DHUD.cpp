// Fill out your copyright notice in the Description page of Project Settings.


#include "DHUD.h"
#include"GameFramework/PlayerController.h"
#include"CharacterOverlay.h"

void ADHUD::BeginPlay()
{
	//继承beginplay(),使用一开始就调用AddCharacterOverlay();
	Super::BeginPlay();

	AddCharacterOverlay();
}

//用于添加血条
void ADHUD::AddCharacterOverlay()
{
	//获取角色，然后在角色上添加血条
	APlayerController* PlayerController = GetOwningPlayerController();
	
	if (PlayerController && CharacterOverlayClass)
	{
		//创建一个 UCharacterOverlay 类型的新小部件实例(OwningPlayer: 这是创建小部件的拥有者,WidgetClass: 这是要创建的小部件类的类型)
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}

}

void ADHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		//将原本的乘上可以设置的扩散
		float SpreadScaled = CrosshairSpreadMax*HUDPackage.CrosshairSpread; 

		//中间准线
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread);
		}

		//右边准线
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);

		}
		//右边准线
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}

		//上面的准线
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		} 

		//下面的准线
		if (HUDPackage.CrosshairsBottom) 
		{
			FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
	}

}

void ADHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X-(TextureWidth/2.f)+Spread.X,
		ViewportCenter.Y-(TextureHight/2.f)+Spread.Y 
		);
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}
