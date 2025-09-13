#include "ControleVaisseau.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraActor.h"
#include "PaperSpriteComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

AControleVaisseau::AControleVaisseau()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

void AControleVaisseau::BeginPlay()
{
	Super::BeginPlay();
	WidgetComp = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);

	if (WidgetComp)
	{
		WidgetComp->AddToViewport();
	}

	Box = FindComponentByClass<UBoxComponent>();
	TriggerBox = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("TriggerB")));
	if (CameraCible)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetViewTargetWithBlend(CameraCible); 
		}
	}
	VieActuelle = VieMax;

	// Ajout du contexte d'input (nécessaire pour Enhanced Input)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

void AControleVaisseau::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AControleVaisseau::Move);
		}

		if (TurnAction)
		{
			EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AControleVaisseau::Turn);
		}
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &AControleVaisseau::Fire);
		}
		
		if (TriggerBox)
		{
			TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AControleVaisseau::OnOverlapBegin);
		}
	}
}
void AControleVaisseau::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DelaiTir += DeltaTime;
}
void AControleVaisseau::Damage()
{
	float TempsActuel = GetWorld()->GetTimeSeconds();

	if (TempsActuel - DernierTempsDegat < TempsInvincibilite)
	{
		return;
	}
	DernierTempsDegat = TempsActuel;
	VieActuelle--;
	if (VieActuelle <= 0)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("Menu")); 
	}
	FLinearColor NewColor;
	if (VieActuelle >= 3)
		NewColor = FLinearColor::Green;
	else if (VieActuelle == 2)
		NewColor = FLinearColor::Yellow;
	else if (VieActuelle == 1)
		NewColor = FLinearColor::Red;
	else
		NewColor = FLinearColor::Black;
	
	if (UPaperSpriteComponent* Sprite = FindComponentByClass<UPaperSpriteComponent>())
	{
		Sprite->SetSpriteColor(NewColor.ToFColor(true));
	}

	
}
void AControleVaisseau::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Magenta, TEXT("Overlap détecté !"));
		Damage();
	}
}


void AControleVaisseau::Move(const FInputActionValue& Value)
{
	
	FVector2D MovementVector = Value.Get<FVector2D>();
	FVector Direction = GetActorForwardVector() * MovementVector.Y + GetActorRightVector() * MovementVector.X;
	Direction.Normalize();
	FVector DeltaMove = Direction * Vitesse * GetWorld()->GetDeltaSeconds();

	// Déplacer l'acteur avec sweep pour gérer les collisions
	FHitResult Hit;
	AddActorWorldOffset(DeltaMove, true, &Hit);
	if (Hit.bBlockingHit)
	{
		Damage();
	}
	
		
	
}

void AControleVaisseau::Turn(const FInputActionValue& Value)
{
	float TurnValue = Value.Get<float>();

	if (FMath::IsNearlyZero(TurnValue))
		return;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw += TurnValue * VitesseRotation * GetWorld()->GetDeltaSeconds();
	SetActorRotation(Rotation);
}

void AControleVaisseau::Fire()
{
	if (BallClass && DelaiTir >= TauxTir) // Vérifie si le cooldown est passé
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();

		// On ajoute +90° en Yaw (autour de l'axe Z)
		FRotator OffsetRotation = SpawnRotation + FRotator(0.f, 90.f, 0.f);

		// Utiliser la rotation modifiée pour la direction
		FVector LaunchDirection = OffsetRotation.Vector(); // Convertit rot -> direction

		AActor* SpawnedBall = GetWorld()->SpawnActor<AActor>(BallClass, SpawnLocation, SpawnRotation);

		if (SpawnedBall)
		{
			UPrimitiveComponent* BallRoot = Cast<UPrimitiveComponent>(SpawnedBall->GetRootComponent());
			if (BallRoot && BallRoot->IsSimulatingPhysics())
			{
				BallRoot->AddImpulse(LaunchDirection * vitessBalle, NAME_None, true);
			}
		}
		DelaiTir = 0.f;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		tireEffect,
		GetActorLocation(),
		GetActorRotation(),
		FVector(1.0f),
		true,
		true);
	}
	
}
