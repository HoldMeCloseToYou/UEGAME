// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DHUD.generated.h"


//��ͼ�ṹ��,������������洢HUDԪ�ص������Ϣ
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

	//���ʮ��׼�߸�������ʹ�ö�̬
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

	//TSubclassOf<class UUserWidget>: ����һ��ģ���࣬
	//��������һ��ָ�� UUserWidget ��,CharacterOverlayClass��
	UPROPERTY(EditAnywhere,Category="Player Stats")
	TSubclassOf<class UUserWidget>CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;

protected:
	virtual void BeginPlay() override;
	void AddCharacterOverlay();



private:
	FHUDPackage HUDPackage;

	//��һ������Ϊ�����ڶ�Ϊ��Ϸ��������
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter,FVector2D Spread);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
};
