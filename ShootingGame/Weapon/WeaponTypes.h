#pragma once


#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252
UENUM(BlueprintType)
enum class EWeaponType:uint8
{
		EWT_AssaultRifle UMETA(DisplayName="Assault Rifle"),
		EWT_RocketLuncher UMETA(DisplayName="Rocket Luncher "),
		EWT_Pistol UMETA(DisplayName="Pistol"),
		EWT_SubmachineGun UMETA(DisplayName="Submachine Gun"),
		EWT_Shotgun UMETA(DisplayName="Shotgun"),
		EWT_SinperRifle UMETA(DisplayName="SinperRifle"),
		EWT_GrenadeLauncher UMETA(DisplayName="GrenadeLauncher"),
		EWT_MAX UMETA(DisplayName="DefaultMAX")
};