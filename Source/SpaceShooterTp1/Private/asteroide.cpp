#include "asteroide.h"
#include "ControleVaisseau.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
Aasteroide::Aasteroide()
{
	PrimaryActorTick.bCanEverTick = true;
	

	// Créer un composant de collision
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(50.f);
	CollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor")); 
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true); 
	RootComponent = CollisionComponent;

	// Créer un mesh pour le visuel
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Stocker l’échelle d’origine
	CurrentScale = FVector(1.f, 1.f, 1.f);
}

// Called when the game starts or when spawned
void Aasteroide::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &Aasteroide::OnHit);
	SetActorScale3D(CurrentScale);
	
}

// Called every frame
void Aasteroide::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fonction appelée en cas de collision
void Aasteroide::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this && OtherComp)
	{
		
		ECollisionChannel ObjectType = OtherComp->GetCollisionObjectType();
		if (ObjectType == ECC_GameTraceChannel1) 
		{
			OtherActor->Destroy();
		}
		if (APawn* Pawn = Cast<APawn>(OtherActor))
		{
			Pawn->CallFunctionByNameWithArguments(TEXT("Damage"), *GLog, nullptr, true);
		}

		Damage();
	}
}

void Aasteroide::Damage()
{
	Health -= FMath::RandRange(1, 2); ;
	CurrentScale *= 0.75f;
	SetActorScale3D(CurrentScale);
	if (Health <= 0)
	{
		AControleVaisseau* MonVaisseau = Cast<AControleVaisseau>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (MonVaisseau)
		{
			 ScoreWidget = MonVaisseau->GetWidgetComp();
			if (ScoreWidget)
			{
				FString Commande = FString::Printf(TEXT("ScorUpdate %d"),1 );
				ScoreWidget->CallFunctionByNameWithArguments(*Commande, *GLog, nullptr, true);
			}
			
		}
		
		FRotator SpawnRotation = GetActorRotation();
		SpawnRotation.Yaw += 180.0f; // Ajoute 90° sur l'axe Z
		SpawnRotation.Pitch += 90.0f; // Rotation sur l'axe X
		SpawnRotation.Roll += 90.0f; 
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		DeathEffect,
		GetActorLocation(),
		SpawnRotation,
		FVector(0.5f),
		true,
		true
	);
		
		Destroy();
	}
}
