#include "LyraAssetManager.h"

#include "LyraLogChannels.h"

ULyraAssetManager::ULyraAssetManager()
{
}

ULyraAssetManager& ULyraAssetManager::Get()
{
	check(GEngine);
	
	if (ULyraAssetManager* Singleton = Cast<ULyraAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	
	UE_LOG(LogLyra, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini. It must be set to LyraAssetManager!"));
	
	return *NewObject<ULyraAssetManager>();
}

const ULyraGameData& ULyraAssetManager::GetGameData()
{
	return GetOrLoadTypedGameData<ULyraGameData>(LyraGameDataPath);
}

UPrimaryDataAsset* ULyraAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass,
	const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	// TODO: Implement this logic
	return nullptr;
}
