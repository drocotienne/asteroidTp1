#pragma once
//teste pour problemme de comit
#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "asteroide.generated.h"
//#include "NiagaraFunctionLibrary.h"


UCLASS()
class SPACESHOOTERTP1_API Aasteroide : public AActor
{
	GENERATED_BODY()
	
public:	
	Aasteroide();

protected:
	virtual void BeginPlay() override;

	// Vies restantes
	UPROPERTY(EditAnywhere, Category = "Asteroide")
	int32 Health = 3;
	
	UPROPERTY(VisibleAnywhere, Category = "Asteroide")
	FVector CurrentScale;
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MeshComponent;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void Damage();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ScoreWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UUserWidget* ScoreWidget;
	UPROPERTY(EditAnywhere, Category = "FX")
	UNiagaraSystem* DeathEffect;

public:	
	virtual void Tick(float DeltaTime) override;
};
