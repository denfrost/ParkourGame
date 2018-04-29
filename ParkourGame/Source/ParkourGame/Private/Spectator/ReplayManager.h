#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ReplayManager.generated.h"

USTRUCT(BlueprintType)
struct FPlayerKeyframe
{
  GENERATED_BODY()

public:

	// World time in seconds
	float WorldTime;

	FTransform RootTransform;

  FVector Velocity;

  int32 IsFullRagdoll;

  bool IsInAir;

	static void LinearInterpolate(const FPlayerKeyframe& A, const FPlayerKeyframe& B, float Alpha, FPlayerKeyframe& Result);
};

class AParkourGameCharacter;
class AGameModeBase;

struct FPlayerReplayData
{
	// reference to the player
  TWeakObjectPtr<AController> PlayerController;
	TWeakObjectPtr<AParkourGameCharacter> PlayerPawn;

	// actor replaying the player
	TWeakObjectPtr<AParkourGameCharacter> ReplayActor;

	// index of the start of the replay keyframes
	int32 CurrentKeyframeIdx = 0;

	// start and end time of the buffer
	float StartTime = 0.0f;
	float EndTime = 0.0f;

	// keyframe data
	TArray<FPlayerKeyframe> Keyframes;

	const FPlayerKeyframe* GetKeyframe(int32 index) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReplayEvent);

UCLASS()
class AReplayManager : public AInfo
{
	GENERATED_BODY()
	
public:

	AReplayManager();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SetRecording(bool bShouldRecord);

	void StartReplay(float ReplayDuration, float ReplayTimeScaling = 1.0f);
	void StopReplay(bool DestroyActors = true);

  FORCEINLINE bool IsRecording() const { return bIsRecording; }
  FORCEINLINE bool IsReplaying() const { return bIsReplaying; }

  // get all actors involved in a replay
  void GetAllReplayActors(TArray<const AActor*>& outActors) const;

public:

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "0.1"))
	float SampleInterval = 0.1f;

	UPROPERTY(EditAnywhere, Category = "Replay Manager", meta = (ClampMin = "1.0"))
	float MaxBufferSize = 10.0f;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnReplayStarted;
  
  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnReplayEnded;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnRecordingStarted;

  UPROPERTY(BlueprintAssignable, Category = "Replay Manager")
  FReplayEvent OnRecordingEnded;

private:

	void StartRecording();
	void StopRecording();

	void OnPlayerLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnPlayerLogout(AGameModeBase* GameMode, AController* Exiting);

	void RecordKeyframe(FPlayerReplayData& Player, float WorldTime);
	void CreateKeyframe(AParkourGameCharacter* Player, FPlayerKeyframe& outKeyframe, float WorldTime);

	void UpdateReplay(float WorldTime);
	void UpdatePlayerReplay(FPlayerReplayData& Player, float ReplayTime);

  void SetupNewPlayer(AController* Controller, AParkourGameCharacter* Player);

  // get the current position of the replay, represented in world time
  float GetCurrentReplayTime(float WorldTime) const;

private:

	FDelegateHandle m_PostLoginHandle;
	FDelegateHandle m_PostLogoutHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsRecording = false;

	UPROPERTY(BlueprintReadOnly, Category = "Replay Manager", meta = (AllowPrivateAccess = "true"))
	bool bIsReplaying = false;

	float ReplayStartTime = 0.0f;
	float ReplayLength = 0.0f;
	float ReplayTimeScale = 0.0f;

	TArray<FPlayerReplayData> m_KeyframeData;

};