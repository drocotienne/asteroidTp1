#include "SpaceShooterTp1/Public/cycltexture.h"


#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UCyclTexture::UCyclTexture()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCyclTexture::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();
	if (MeshComponent && MeshComponent->GetMaterial(0))
	{
		DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	}
	
}

void UCyclTexture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DynamicMaterial)
	{
		Offset = Direction.GetSafeNormal() * GetWorld()->GetTimeSeconds() * ScrollSpeed;
		DynamicMaterial->SetVectorParameterValue(TextureOffsetParameterName, FLinearColor(Offset.X, Offset.Y, 0.0f, 0.0f));
	}
}
