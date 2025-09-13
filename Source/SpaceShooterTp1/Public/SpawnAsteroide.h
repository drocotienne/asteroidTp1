#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnAsteroide.generated.h"

UCLASS()
class SPACESHOOTERTP1_API ASpawnAsteroide : public AActor
{
	GENERATED_BODY()

public:
	ASpawnAsteroide();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	AActor* SpawnSourceActor;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> AsteroidBlueprint;
	float tempDaparition =4.0f;
	float Delaidaparition = 0.f;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> PlayerClass;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	int32 NumberOfAsteroids = 10;

private:
	AActor* TargetPlayer;
	class UBoxComponent* SpawnBox;

	TArray<AActor*> SpawnedAsteroids;

	void SpawnAsteroids();
	void PropelAsteroid(AActor* Asteroid);
	FVector GetRandomPointOnBoxEdge(UBoxComponent* Box);
};
