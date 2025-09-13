#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "WrapTp.generated.h"

UCLASS()
class SPACESHOOTERTP1_API AWrapTp : public AActor
{
	GENERATED_BODY()

public:
	AWrapTp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	float MinX ;
	float MaxX ;
	float MinY ;
	float MaxY ;

	// Composant Box pour définir la zone de wrap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrap")
	UBoxComponent* BoxComponent;

	// Marge pour ne pas téléporter trop tôt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrap")
	float Margin = 50.f;

	// Caméra orthographique à partir de laquelle on calcule la taille du wrap
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrap")
	ACameraActor* OrthoCamera;

	// Facteur d'ajustement sur l'axe Z (car OrthoWidth ne couvre que X/Y)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wrap")
	float OrthoDepth = 1000.f;
	UPROPERTY()
	TSet<AActor*> RecentlyTeleportedActors;
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
