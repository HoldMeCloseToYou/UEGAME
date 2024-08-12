// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BulletHitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBulletHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTINGGAME_API IBulletHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable) 是函数修饰符，表示这是一个蓝图原生事件函数，并且可以在蓝图中被调用。
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void BulletHit(FHitResult HitResult,AActor* Shooter,AController*Instigator);
};
