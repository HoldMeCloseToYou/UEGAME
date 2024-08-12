#pragma once

#include "CoreMinimal.h"
#include "projectile.h"
#include "projectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGGAME_API AprojectileRocket : public Aprojectile
{
	GENERATED_BODY()
	

public:
	AprojectileRocket();
	virtual void Destroyed() override;
protected:
		//(碰撞盒，角色，其他被击中的组件，3d空间垂直，击中结果）
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
		virtual void BeginPlay() override;
		

		UPROPERTY(EditAnywhere)
		USoundCue* ProjectileLoop;


		//循环声音，即火箭弹在飞行过程的声音
		UPROPERTY()
		UAudioComponent* ProjectileLoopComponent;
		
		//循环声音衰减
		UPROPERTY(EditAnywhere)
		USoundAttenuation* LoopingSoundAttenuation;

		UPROPERTY(VisibleAnywhere)
		class URocketMovementComponent* RocketMovementComponent;
private:
	
	
};
