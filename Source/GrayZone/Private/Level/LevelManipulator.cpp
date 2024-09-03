// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LevelManipulator.h"

// Sets default values
ALevelManipulator::ALevelManipulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    this->m_dungeonGenerator = CreateDefaultSubobject<UDungeonGeneratorComponent>(TEXT("Dungeon Generator Component"));
    this->m_dungeonDecorator = CreateDefaultSubobject<UDungeonDecoratorComponent>(TEXT("Dungeon Decorator Component"));

    m_playerCharacter = TSubclassOf<APlayerCharacter>();
    m_spawnedPlayer   = nullptr;
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

    //TO DO: in the future the spawn position will be set manually inside the spawn room and then we will be getting it from the Room itself. 
    auto spawnPos = this->m_dungeonGenerator->GetSpawnRoomData()->GetRealCenterPos();
    this->SpawnOrEnablePlayer(FVector(spawnPos.X, spawnPos.Y, 95));

    //We spawn the camera and focus it on the player.
    if (!this->m_cameraType)
    {
        UE_LOG(LogTemp, Fatal, TEXT("No camera type was specified for the player character to spawn."));
        return;
    }

    if (this->m_camera == nullptr) this->m_camera = this->GetWorld()->SpawnActor<ATargetCamera>(this->m_cameraType);
    this->m_camera->SetTarget(this->m_spawnedPlayer);
}


void ALevelManipulator::SpawnOrEnablePlayer(FVector spawnPos)
{
    if (this->m_spawnedPlayer == nullptr) 
    {
        this->m_spawnedPlayer = this->GetWorld()->SpawnActor<APlayerCharacter>(this->m_playerCharacter, spawnPos, FRotator::ZeroRotator);

        //We initialize the forward and right vectors for player movement using the camera rotation (the camera rotation is always fixed).
        auto cameraRotation = ATargetCamera::CAMERA_ROTATION;
        cameraRotation.Pitch = 0; //We don't want the player to move up or down.

        this->m_spawnedPlayer->Initialize(cameraRotation.Vector(), FRotationMatrix(cameraRotation).GetScaledAxis(EAxis::Y));
    }
    else 
    {
        this->m_spawnedPlayer->SetActorLocationAndRotation(spawnPos, FRotator::ZeroRotator);
        m_spawnedPlayer->SetActorHiddenInGame(false);
    }
}

void ALevelManipulator::CleanLevel()
{
    if (m_spawnedPlayer != nullptr) m_spawnedPlayer->SetActorHiddenInGame(true); //If there's a spawned player, we hide it.

    this->m_dungeonDecorator->Clean();
    this->m_dungeonGenerator->Clean();
}

