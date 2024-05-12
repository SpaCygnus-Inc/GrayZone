// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelManipulator.h"

// Sets default values
ALevelManipulator::ALevelManipulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->m_dungeonGenerator = CreateDefaultSubobject<UDungeonGeneratorComponent>(TEXT("Dungeon Generator Component"));
    this->m_dungeonDecorator = CreateDefaultSubobject<UDungeonDecoratorComponent>(TEXT("Dungeon Decorator Component"));
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

    //We use the current ticks as a seed for creating a map. 
    this->m_dungeonSeed = FDateTime::Now().GetTicks();
    FMath::RandInit(this->m_dungeonSeed);

    this->m_dungeonGenerator->GenerateDungeon();
    this->m_dungeonDecorator->DecorateDungeon(this->m_dungeonGenerator, this);
}

void ALevelManipulator::CleanLevel()
{
    this->m_dungeonDecorator->Clean();
    this->m_dungeonGenerator->Clean();
}

