#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CyclTexture.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACESHOOTERTP1_API UCyclTexture : public UActorComponent
{
	GENERATED_BODY()

public:    
	UCyclTexture();
	
protected:
	virtual void BeginPlay() override;
	
public:    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, Category = "Texture Scroll")
	float ScrollSpeed = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Texture Scroll")
	FVector2D Direction = FVector2D(1.0f, 3.0f);
	
private:
	FVector2D Offset;
	UMaterialInstanceDynamic* DynamicMaterial;
	UMeshComponent* MeshComponent;
	FName TextureOffsetParameterName = "UVOffset";
};
