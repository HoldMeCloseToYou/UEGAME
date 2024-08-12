// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyAssetManager.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnPackageLoaded);

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API UMyAssetManager : public UAssetManager
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName CurrentLoadPackage;

public:
	UFUNCTION(BlueprintPure, CallInEditor, DisplayName = "MyAssetManager")
		static UMyAssetManager* Get();

	UFUNCTION(BlueprintCallable, CallInEditor)
		void MyAsyncLoadObject(FString path, FOnPackageLoaded OnPackageLoaded);

	UFUNCTION(BlueprintCallable, CallInEditor)
		float GetCurrentLoadProgress(FName PackageName, int32& LoadedCount, int32& RequestedCount) const;
};
