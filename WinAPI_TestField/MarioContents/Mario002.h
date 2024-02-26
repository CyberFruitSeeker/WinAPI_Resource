#pragma once
#include <EngineCore/Actor.h>
#include "ContentsHelper.h"
#include <EngineCore/Collision.h>
#include "Physics.h"

// 설명 :
class Mario : public Physics
{
public:
	static FVector MarioLocation;

	// constrcuter destructer
	Mario();
	~Mario();

	// delete Function
	Mario(const Mario& _Other) = delete;
	Mario(Mario&& _Other) noexcept = delete;
	Mario& operator=(const Mario& _Other) = delete;
	Mario& operator=(Mario&& _Other) noexcept = delete;


protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void MoveLastCamera();


	void SetState(PlayerState _State);
	void StateUpdate(float _DeltaTime);

	void IncreaseSpeed(float _DeltaTime, FVector _Fvector);
	void DecreaseSpeed(float _DeltaTime, FVector _Fvector);


	// 상태 시작, 키입력의 기능이 담긴 함수들
	// Fly도 필요한지는 추후 판별
	void Idle(float _DeltaTime);
	void Jump(float _DeltaTime);
	void Run(float _DeltaTime);

	void IdleStart();
	void RunStart();
	void JumpStart();

	void DirChange(float _DeltaTime);
	void DirChangeStart();

	void DieStart();
	void Die(float _DeltaTime);

	void Interactive(float _DeltaTime);
	void InteractiveStart();

	void JustRun(float _DeltaTime, FVector Accel);


	// 마리오가 맵의 컬리전에 끼이는 현상을 확인하면
	// 엣지 체크 함수 추가 필요
	void DirCheck();

	void MarioColEffect(float _DeltaTime);

	void MoveResult(float _DeltaTime) override;

	//void SetAnimation(std::string _Name) override;

	const FVector AccelX = { 250.f,0.f,0.f,0.f };
	const FVector AccelY = { 0.f,1000.f,0.f,0.f };
	const FVector BreakAccelX = AccelX * 2;
	const FVector DirChangeAccelX = BreakAccelX * 2;

	bool DirChanging = false;

	const float MinSpeed = 5.f;
	const float MaxSpeedX = 500.f;
	const float  MaxSpeedY = 1000.f;
	int CurSpeedDir = 0;
	const float JumpPower = -1000.f;
	bool Jumping = false;

	float DeadTime = .5f;


	// 마리오 동작들 가속도 설정

	// Physics 클래스에
	// UImageRenderer* Renderer = nullptr;
	// UImageRenderer* ColRenderer = nullptr;
	// 가 있다는 것에 주의

	//UCollision* BodyCollision = nullptr;
	//UImageRenderer* Renderer = nullptr;

	// 마리오에게 적용되는 중력과 중력 가속도


	// 점프, 나아갈 모든 방향의 힘의 합


	// 이동 관련 중력 연산


private:
	PlayerState State = PlayerState::None;

};
