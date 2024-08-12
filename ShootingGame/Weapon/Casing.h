// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class SHOOTINGGAME_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh;

	//给子弹壳定义一个初速度的属性
	UPROPERTY(EditAnywhere)
		float ShellEjectionImpuse;

	//弹壳的声音
	UPROPERTY(EditAnywhere)
		class USoundCue* ShellSound;

	FTimerHandle DestroyTimerHandle; // 添加一个计时器句柄
	

};
