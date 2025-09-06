// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/WeaponTypes.h"
#include "Weapon.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
class ACasing;
class UNiagaraComponent;
class UWidgetComponent;
class USphereComponent;
class USkeletalMeshComponent;

UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	void ShowPickupWidget(const bool bShowWidget);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void SetHUDAmmo();
	void AddAmmo(int32 AmmoToAdd);

	/** ~Begin Getter & Setter */
	void SetWeaponState(const EWeaponState State);
	EWeaponState GetWeaponState() const {return WeaponState;}
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	int32 GetAmmo() const { return Ammo; }
	int32 GetMagCapacity() const { return MagCapacity; }
	bool IsAmmoEmpty();
	/** ~End Getter & Setter */

#pragma region Crosshairs
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsCenter = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsLeft = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsRight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsTop = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	TSoftObjectPtr<UTexture2D> CrosshairsBottom = nullptr;
#pragma endregion

	/**
	 * Zoomed FOV while aiming
	 */
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/**
	 * Automatic fire
	 */

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> EquipSound;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "Fire Effect")
	TObjectPtr<UNiagaraComponent> MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Effect")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Fire Effect")
	FName MuzzleName = "Muzzle";

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

private :
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere,  Category = "Weapon Properties")
	EWeaponState WeaponState = EWeaponState::EWS_Initial;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACasing> CasingClass;

#pragma region Ammo
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo)
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

#pragma endregion
	UPROPERTY()
	ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;
};
