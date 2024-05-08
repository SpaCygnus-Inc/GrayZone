// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelManipulator.h"

// Sets default values
ALevelManipulator::ALevelManipulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->m_dungeonGenerator = CreateDefaultSubobject<UDungeonGenerator>(TEXT("Dungeon Generator Component"));
}

// Called when the game starts or when spawned
void ALevelManipulator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelManipulator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelManipulator::GenerateLevel()
{
    this->CleanLevel();
    this->m_dungeonGenerator->GenerateDungeon();
}

void ALevelManipulator::CleanLevel()
{
    this->m_dungeonGenerator->CleanDungeon();
}

