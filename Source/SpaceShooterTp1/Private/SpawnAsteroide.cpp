#include "SpawnAsteroide.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"

ASpawnAsteroide::ASpawnAsteroide()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASpawnAsteroide::BeginPlay()
{
    Super::BeginPlay();

    // 🔍 Trouver le joueur
    if (PlayerClass)
    {
        for (TActorIterator<AActor> It(GetWorld(), PlayerClass); It; ++It)
        {
            TargetPlayer = *It;
            break;
        }
    }
    else
    {
        TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    }

    // 🔍 Récupérer la BoxComponent de l’acteur externe
    if (SpawnSourceActor)
    {
        TArray<UBoxComponent*> BoxComponents;
        SpawnSourceActor->GetComponents<UBoxComponent>(BoxComponents);

        if (BoxComponents.Num() > 0)
        {
            SpawnBox = BoxComponents[0];
        }
    }

    if (!SpawnBox || !AsteroidBlueprint)
    {
        UE_LOG(LogTemp, Error, TEXT("ASpawnAsteroide: SpawnBox or AsteroidBlueprint is missing."));
        return;
    }

    SpawnAsteroids();
}

void ASpawnAsteroide::SpawnAsteroids()
{
    for (int32 i = 0; i < NumberOfAsteroids; ++i)
    {
        FVector SpawnLocation = GetRandomPointOnBoxEdge(SpawnBox);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AActor* NewAsteroid = GetWorld()->SpawnActor<AActor>(AsteroidBlueprint, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewAsteroid)
        {
            SpawnedAsteroids.Add(NewAsteroid);
            PropelAsteroid(NewAsteroid);
            Delaidaparition = 0.f;
        }
    }
}

void ASpawnAsteroide::PropelAsteroid(AActor* Asteroid)
{
    if (!Asteroid || !TargetPlayer) return;

    UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Asteroid->GetRootComponent());
    if (RootPrimitive && RootPrimitive->IsSimulatingPhysics())
    {
        FVector Direction = (TargetPlayer->GetActorLocation() - Asteroid->GetActorLocation()).GetSafeNormal();
        float Force = 200.f;

        RootPrimitive->AddImpulse(Direction * Force, NAME_None, true);
    }
}

void ASpawnAsteroide::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Delaidaparition >= tempDaparition) 
    {
        SpawnAsteroids();
        Delaidaparition = 0.f;
    }
    Delaidaparition+= DeltaTime;
}


FVector ASpawnAsteroide::GetRandomPointOnBoxEdge(UBoxComponent* Box)
{
    if (!Box || !TargetPlayer) return FVector::ZeroVector;

    FVector Extent = Box->GetScaledBoxExtent();
    FVector Origin = Box->GetComponentLocation();
    
    float PlayerZLocal = Box->GetComponentTransform().InverseTransformPosition(TargetPlayer->GetActorLocation()).Z;

    bool spawnOnXEdge = FMath::RandBool();

    FVector PointLocal;

    if (spawnOnXEdge)
    {
        float xEdge = FMath::RandBool() ? Extent.X : -Extent.X;
        float yPos = FMath::FRandRange(-Extent.Y, Extent.Y);
        PointLocal = FVector(xEdge, yPos, PlayerZLocal-1);
    }
    else
    {
        float yEdge = FMath::RandBool() ? Extent.Y : -Extent.Y;
        float xPos = FMath::FRandRange(-Extent.X, Extent.X);
        PointLocal = FVector(xPos, yEdge, PlayerZLocal-1);
    }
    UE_LOG(LogTemp, Warning, TEXT("asteroide suposer spawn !"));
    return Box->GetComponentTransform().TransformPosition(PointLocal);
}