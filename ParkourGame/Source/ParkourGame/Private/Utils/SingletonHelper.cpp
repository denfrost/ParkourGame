#include "SingletonHelper.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "../MiniGame/MiniGameManager.h"
#include "../ParkourGameCharacter.h"

FSingletonHelper::FSingletonHelper()
{
}

FSingletonHelper::~FSingletonHelper()
{
}

AActor* FSingletonHelper::GetSingletonObjectByClass(UWorld* WorldPtr, UClass* Class)
{
	if (!Class) return nullptr;

	//search for the object in our existing list
	TWeakObjectPtr<AActor>* FoundObject = ObjectPtrs.Find(Class);

	if (FoundObject)
	{
		if (AActor* ExistingObject = FoundObject->Get()) return ExistingObject;
	}

	if (!WorldPtr) return nullptr;

	// otherwise try to find the object in the world
	for (TActorIterator<AActor> It(WorldPtr, Class); It; ++It)
	{
		if (AActor* FoundActor = *It)
		{
			ObjectPtrs.Add(Class, FoundActor);
			return FoundActor;
		}
	}

	return nullptr;
}



class AMiniGameManager* USingletonHelperLibrary::GetMiniGameManager(UObject* WorldContextObject)
{
	if(AParkourGameCharacter* Char = Cast<AParkourGameCharacter>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0)))
		return Char->SingletonHelper->GetSingletonObject<AMiniGameManager>(Char->GetWorld());

	return nullptr;
}