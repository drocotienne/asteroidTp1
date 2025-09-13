// Fill out your copyright notice in the Description page of Project Settings.


#include "GitProblemeTeste.h"

GitProblemeTeste::GitProblemeTeste()
{
    
}

GitProblemeTeste::~GitProblemeTeste()
{
}
void GitProblemeTeste::BeginPlay()
{
    Super::BeginPlay();
    CollisionComponent->OnComponentHit.AddDynamic(this, &Aasteroide::OnHit);
    SetActorScale3D(CurrentScale);
	
}