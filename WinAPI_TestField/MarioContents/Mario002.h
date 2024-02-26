#pragma once
#include <EngineCore/Actor.h>
#include "ContentsHelper.h"
#include <EngineCore/Collision.h>
#include "Physics.h"

// ���� :
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


	// ���� ����, Ű�Է��� ����� ��� �Լ���
	// Fly�� �ʿ������� ���� �Ǻ�
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


	// �������� ���� �ø����� ���̴� ������ Ȯ���ϸ�
	// ���� üũ �Լ� �߰� �ʿ�
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


	// ������ ���۵� ���ӵ� ����

	// Physics Ŭ������
	// UImageRenderer* Renderer = nullptr;
	// UImageRenderer* ColRenderer = nullptr;
	// �� �ִٴ� �Ϳ� ����

	//UCollision* BodyCollision = nullptr;
	//UImageRenderer* Renderer = nullptr;

	// ���������� ����Ǵ� �߷°� �߷� ���ӵ�


	// ����, ���ư� ��� ������ ���� ��


	// �̵� ���� �߷� ����


private:
	PlayerState State = PlayerState::None;

};
