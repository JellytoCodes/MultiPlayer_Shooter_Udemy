// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Casing.h"
#include "Kismet/GameplayStatics.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulse = 100.f;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShellSound, GetActorLocation());
	Destroy();
}
