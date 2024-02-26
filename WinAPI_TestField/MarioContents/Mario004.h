#pragma once
#include <EngineCore/Actor.h>
#include "ContentsHelper.h"
#include <EngineCore/Collision.h>
#include "Physics.h"

// Ό³Έν :
class Mario : public Physics
{
public:

	// constrcuter destructer
	Mario();
	~Mario();

	// delete Function
	Mario(const Mario& _Other) = delete;
	Mario(Mario&& _Other) noexcept = delete;
	Mario& operator=(const Mario& _Other) = delete;
	Mario& operator=(Mario&& _Other) noexcept = delete;

	void SetState(PlayerState _State);

	void SetMarioClassState(MarioModClass _MarioClass);

	static FVector MarioLocation;

	static MarioModClass MyMarioClass;



protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void SetCameraLastMovePos();

	void StateUpdate(float _DeltaTime) override;
	void SpeedUp(float _DeltaTime, FVector _FVector);
	void SpeedDown(float _DeltaTime, FVector _FVector);


	void Idle(float _DeltaTime);
	void Move(float _DeltaTime);
	void Jump(float _DeltaTime);

	void IdleStart();
	void MoveStart();
	void JumpStart();

	void JustMove(float _DeltaTime, FVector Acclerate);

	void DirChangeStart();
	void InteractiveStart();
	void Interactive(float _DeltaTime);

	void Dead(float _DeltaTime);
	void DeadStart();
	void EndStart();
	void EndMoveStart();

	void ChangingStart();
	void DirChange(float _DeltaTime);

	void End(float _DeltaTime);
	void EndMove(float _DeltaTime);
	void Changing(float _DeltaTime);

	bool LeftEdgeCheck();
	bool RightEdgeCheck();
	void DirCheck();

	void MarioChange(bool _Positive);

	void MarioCollision(float _DeltaTime);

	void ResultMove(float _DeltaTime) override;

	void SetAnimation(std::string _Name) override;

	const FVector AccelerateX = { 2500.f,0.f,0.f,0.f };
	const FVector AccelerateY = { 0.f,3500.f,0.f,0.f };
	const FVector StopAccelerateX = AccelerateX * 2;
	const FVector DirChangeAccelerateX = StopAccelerateX * 2;


	bool DirChanging = false;

	const float MinSpeed = 5.f;
	const float MaxSpeedX = 500.f;
	const float  MaxSpeedY = 1000.f;
	int CurSpeedDir = 0;

	const float JumpPower = -750.f;
	bool Jumping = false;

	float DeadTime = .5f;

	float EndTime = 0.5f;

	float ChangeTime = 0.6f;
	float GodTime = 3.f;

private:
	PlayerState State = PlayerState::None;
	PlayerState PrevState = PlayerState::None;
};
