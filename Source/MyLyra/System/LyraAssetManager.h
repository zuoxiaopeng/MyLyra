#pragma once
#include "LyraGameData.h"
#include "Engine/AssetManager.h"
#include "LyraAssetManager.generated.h"

#define UE_API MYLYRA_API

UCLASS(MinimalAPI, Config = Game)
class ULyraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	
	UE_API ULyraAssetManager();
	
	static UE_API ULyraAssetManager& Get();
	
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);
	
	UE_API const ULyraGameData& GetGameData();
	
protected:
	template<typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (const TObjectPtr<UPrimaryDataAsset>* pResult = GameDataMap.Find(GameDataClass::StaticClass))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}
		
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}
	
	UE_API UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);
	
protected:
	UPROPERTY(Config)
	TSoftObjectPtr<ULyraGameData> LyraGameDataPath;
	
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;
};

template<typename AssetType>
TSubclassOf<AssetType> ULyraAssetManager::GetSubClass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	// TODO: Implement this logic
	TSubclassOf<AssetType> LoadedSubclass;
	return LoadedSubclass;
}


#undef UE_API