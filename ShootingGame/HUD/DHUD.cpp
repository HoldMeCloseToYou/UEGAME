// Fill out your copyright notice in the Description page of Project Settings.


#include "DHUD.h"
#include"GameFramework/PlayerController.h"
#include"CharacterOverlay.h"

void ADHUD::BeginPlay()
{
	//�̳�beginplay(),ʹ��һ��ʼ�͵���AddCharacterOverlay();
	Super::BeginPlay();

	AddCharacterOverlay();
}

//�������Ѫ��
void ADHUD::AddCharacterOverlay()
{
	//��ȡ��ɫ��Ȼ���ڽ�ɫ�����Ѫ��
	APlayerController* PlayerController = GetOwningPlayerController();
	
	if (PlayerController && CharacterOverlayClass)
	{
		//����һ�� UCharacterOverlay ���͵���С����ʵ��(OwningPlayer: ���Ǵ���С������ӵ����,WidgetClass: ����Ҫ������С�����������)
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

		//��ԭ���ĳ��Ͽ������õ���ɢ
		float SpreadScaled = CrosshairSpreadMax*HUDPackage.CrosshairSpread; 

		//�м�׼��
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread);
		}

		//�ұ�׼��
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);

		}
		//�ұ�׼��
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}

		//�����׼��
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread);
		} 

		//�����׼��
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
