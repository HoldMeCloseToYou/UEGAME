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
		//(��ײ�У���ɫ�����������е������3d�ռ䴹ֱ�����н����
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
		virtual void BeginPlay() override;
		

		UPROPERTY(EditAnywhere)
		USoundCue* ProjectileLoop;


		//ѭ����������������ڷ��й��̵�����
		UPROPERTY()
		UAudioComponent* ProjectileLoopComponent;
		
		//ѭ������˥��
		UPROPERTY(EditAnywhere)
		USoundAttenuation* LoopingSoundAttenuation;

		UPROPERTY(VisibleAnywhere)
		class URocketMovementComponent* RocketMovementComponent;
private:
	
	
};
