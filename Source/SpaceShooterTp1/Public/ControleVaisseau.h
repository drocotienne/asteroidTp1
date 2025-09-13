#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NiagaraSystem.h"
#include "InputAction.h"             // Pour UInputAction
#include "InputMappingContext.h"     // Pour UInputMappingContext
#include "ControleVaisseau.generated.h"


class UBoxComponent;
class ABalle;

UCLASS(Blueprintable)
class SPACESHOOTERTP1_API AControleVaisseau : public APawn
{
	GENERATED_BODY()

public:
	AControleVaisseau();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Caméra")
	ACameraActor* CameraCible;

	// Composant FloatingPawnMovement
	UPROPERTY(VisibleAnywhere, Category = "Mouvement")
	class UFloatingPawnMovement* FloatingPawnMovement;

	// Input Actions
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* TurnAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* FireAction;

	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMappingContext;

	// Santé
	UPROPERTY(EditAnywhere, Category = "Joueur")
	float TempsInvincibilite = 0.2f;
	float DernierTempsDegat = -1000.f;
	UPROPERTY(EditAnywhere, Category = "Joueur")
	int VieMax = 3;
	int VieActuelle;

	// Mouvement
	UBoxComponent* Box = nullptr;
	UPROPERTY(EditAnywhere, Category = "Mouvement")
	float Vitesse = 250.f;
	UPROPERTY(EditAnywhere, Category = "Mouvement")
	float VitesseRotation = 250.f;

	// Tir
	UPROPERTY(EditAnywhere, Category = "Tir")
	TSubclassOf<AActor> BallClass;
	UPROPERTY(EditAnywhere, Category = "Tir")
	float TauxTir = 0.5f;
	float DelaiTir = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Tir")
	float DureeVieBalle = 2.f;
	UPROPERTY(EditAnywhere, Category = "Tir")
	float  vitessBalle = 600.f;
	UPROPERTY(VisibleAnywhere, Category = "box")
	UBoxComponent* TriggerBox;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
	
	void Damage();

	// Fonctions de contrôle (pour Enhanced Input)
	void Move(const struct FInputActionValue& Value);
	void Turn(const struct FInputActionValue& Value);
	void Fire();
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> WidgetClass;
	UPROPERTY(VisibleAnywhere, Category="UI")
	UUserWidget* WidgetComp;
	UUserWidget* GetWidgetComp() const { return WidgetComp; }

	UPROPERTY(EditAnywhere, Category = "FX")
	UNiagaraSystem* tireEffect;

	
};
