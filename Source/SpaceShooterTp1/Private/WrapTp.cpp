#include "WrapTp.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
AWrapTp::AWrapTp()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));

	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWrapTp::OnOverlapEnd);
}

void AWrapTp::BeginPlay()
{
	Super::BeginPlay();
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);
	 MinX = Origin.X - BoxExtent.X ;
	 MaxX = Origin.X + BoxExtent.X ;
	 MinY = Origin.Y - BoxExtent.Y ;
	 MaxY = Origin.Y + BoxExtent.Y ;

	if (!OrthoCamera)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWrapTp: OrthoCamera non assignée."));
		return;
	}

	UCameraComponent* CameraComponent = OrthoCamera->GetCameraComponent();

	if (!CameraComponent || CameraComponent->ProjectionMode != ECameraProjectionMode::Orthographic)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWrapTp: La caméra n'est pas en mode orthographique."));
		return;
	}
	
	const float OrthoWidth = CameraComponent->OrthoWidth;
	const float HalfWidth = OrthoWidth * 0.5f;
	
	const float AspectRatio = CameraComponent->AspectRatio;
	const float OrthoHeight = OrthoWidth / AspectRatio;
	const float HalfHeight = OrthoHeight * 0.5f;
	
	DrawDebugBox(
		GetWorld(),
		BoxComponent->GetComponentLocation(),
		BoxComponent->GetScaledBoxExtent(),
		FQuat::Identity,
		FColor::Red,
		true,
		-1.f,
		0,
		2.f
	);
}


void AWrapTp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWrapTp::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
		return;
	ECollisionChannel ObjectType = OtherComp->GetCollisionObjectType();

	if (ObjectType == ECC_GameTraceChannel1) 
	{
		
		OtherActor->Destroy();
	}
	// Si l'acteur a déjà été téléporté récemment, on ignore
	if (RecentlyTeleportedActors.Contains(OtherActor))
		return;

	/*FVector Origin;
	FVector BoxExtent;
	GetActorBounds(false, Origin, BoxExtent);

	float MinX = Origin.X - BoxExtent.X ;
	float MaxX = Origin.X + BoxExtent.X ;
	float MinY = Origin.Y - BoxExtent.Y ;
	float MaxY = Origin.Y + BoxExtent.Y ;*/
	FVector Location = OtherActor->GetActorLocation();
	bool bTeleported = false;
	float Offset = 0.f;

	if (Location.X < MinX+Offset)
	{
		Location.X = MaxX - Offset;
		bTeleported = true;
	}
	else if (Location.X > MaxX-Offset)
	{
		Location.X = MinX + Offset;
		bTeleported = true;
	}

	if (Location.Y < MinY-Offset)
	{
		Location.Y = MaxY - Offset;
		bTeleported = true;
	}
	else if (Location.Y > MaxY+Offset)
	{
		Location.Y = MinY + Offset;
		bTeleported = true;
	}

	if (bTeleported)
	{
		OtherActor->SetActorLocation(Location);

		// Marquer comme récemment téléportéw
		RecentlyTeleportedActors.Add(OtherActor);

		// Supprimer après un court délai (0.1s)
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, OtherActor]()
		{
			RecentlyTeleportedActors.Remove(OtherActor);
		}, 0.1f, false);
	}
}

