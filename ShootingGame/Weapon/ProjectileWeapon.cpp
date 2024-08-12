// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
//用于MuzzleFlashSocket
#include"Engine/SkeletalMeshSocket.h"
#include"projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	//这里用于产生射弹
	Super::Fire(HitTarget);
	//这里先获取后，接着后面的if用于判断是否有效
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	//USkeletalMeshSocket 类通常用于表示骨骼网格模型中的特定位置
	const USkeletalMeshSocket* MuzzleFlashSocket =GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	
	if (MuzzleFlashSocket&&World)
	{
		
		//这里的GetSocketTransform需要socket所属的骨架网格
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		//从枪口尖端到，红色准星击中的位置
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		//将上面计算的指定方向转换为旋转角度
		FRotator TargetRotation = ToTarget.Rotation();


		//如果存在projectclass即子弹和角色是有效的
		if (ProjectClass&&InstigatorPawn)
		{

		
			FActorSpawnParameters SpawnParams;
			//设置发射武器的角色是弹丸的拥有者
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			Aprojectile* SpawnedProjectile = nullptr;
			if (World)
			{
				World->SpawnActor<Aprojectile>(
					//弹丸类
					ProjectClass,
					//获取位置
					SocketTransform.GetLocation(),
					//需要偏移的位置
					TargetRotation,
					//SpawnParams 是一个结构体，用于指定生成弹丸时的各种参数
					SpawnParams
				);
			}

		}
	}
}
