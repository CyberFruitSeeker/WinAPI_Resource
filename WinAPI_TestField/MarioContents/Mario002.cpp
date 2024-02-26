#include "Mario.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineBase/EngineDebug.h>
#include <EngineCore/Collision.h>
#include <string>
#include <vector>
#include <list>
#include "Physics.h"
#include "ContentsHelper.h"

FVector Mario::MarioLocation = {};

Mario::Mario()
{
	DirState = EActorDir::Right;
}

Mario::~Mario()
{
}

void Mario::BeginPlay()
{
	AActor::BeginPlay();
	//ItsMeMario = this;
	{
		Renderer = CreateImageRenderer(MarioRenderOrder::Player);
		SetName("Mario");
		Renderer->SetImage("Mario_Right.png");
		Renderer->SetTransform({ {0,0}, {256, 256} });

		AutoAni(Renderer, "Idle", 0, 0);
		AutoAni(Renderer, "Move", 1, 3);
		AutoAni(Renderer, "DirChange", 4, 4);
		AutoAni(Renderer, "Jump", 5, 5);
		//AutoAni(Renderer, "Die", 6, 6);
		AutoAni(Renderer, "End", 7, 7);




		//Renderer->CreateAnimation("Idle_Right", "Player_Right.png", 0, 0, 0.45f, true);
		//Renderer->CreateAnimation("Idle_Left", "Player_Left.png", 0, 0, 0.45f, true);

		//Renderer->CreateAnimation("Run_Right", "Player_Right.png", { 1, 2, 3 }, 0.1f, true);
		//Renderer->CreateAnimation("Run_Left", "Player_Left.png", { 1, 2, 3 }, 0.1f, true);

		//Renderer->CreateAnimation("Jump_Right", "Player_Right.png", 5, 5, 0.1f, true);
		//Renderer->CreateAnimation("Jump_Left", "Player_Left.png", 5, 5, 0.1f, true);

		SetAnimation("Idle");
		SetState(PlayerState::Idle);

	}



	SetState(PlayerState::Idle);
}


void Mario::Tick(float _DeltaTime)
{
	AActor::Tick(_DeltaTime);

	MarioLocation = GetActorLocation();

}

// ====== 마리오의 움직임에 따른 카메라 이동 ======

void Mario::MoveLastCamera()
{
	// 카메라는 x축으로만 움직이게 해본다.
	FVector CurPos = GetActorLocation();
	FVector CurCameraPos = GetWorld()->GetCameraPos();
	float WindowCenter = GEngine->MainWindow.GetWindowScale().hX();
	if (CurPos.X > WindowCenter + CurCameraPos.X) {
		GetWorld()->SetCameraPos({ CurPos.X - WindowCenter,CurCameraPos.Y });
	}
}



// ======== 마리오의 움직임 기능, 셋팅, 키입력 ===========

void Mario::SetState(PlayerState _State)
{
	if (State != _State || _State == PlayerState::Interactive)
	{
		switch (_State)
		{
		case PlayerState::Idle:
			IdleStart();
			break;
		case PlayerState::Run:
			RunStart();
			break;
		case PlayerState::Jump:
			JumpStart();
			break;
		case PlayerState::DirChange:
			DirChangeStart();
			break;
		case PlayerState::Interactive:
			InteractiveStart();
			break;
		case PlayerState::PlayerDie:
			DieStart();
			break;
		default:
			break;
		}
	}
	State = _State;
}

void Mario::StateUpdate(float _DeltaTime)
{
	DirCheck();
	switch (State)
	{
	case PlayerState::None:
		break;
	case PlayerState::Idle:
		GravityCheck(_DeltaTime);
		Idle(_DeltaTime);
		break;
	case PlayerState::Run:
		GravityCheck(_DeltaTime);
		Run(_DeltaTime);
		break;
	case PlayerState::Jump:
		GravityCheck(_DeltaTime);
		Jump(_DeltaTime);
		break;
	case PlayerState::DirChange:
		GravityCheck(_DeltaTime);
		DirChange(_DeltaTime);
		break;
	case PlayerState::Interactive:
		GravityCheck(_DeltaTime);
		Interactive(_DeltaTime);
		break;
	case PlayerState::PlayerDie:
		GravityCheck(_DeltaTime);
		Die(_DeltaTime);
		break;
	default:
		break;
	}

}

// ======== idle, jump, run ==============


void Mario::Idle(float _DeltaTime)
{
	MoveResult(_DeltaTime);

	if (0 == CurSpeedDir)
	{
		XSpeed.X = 0;
	}

	if (UEngineInput::IsPress(VK_CONTROL))
	{
		SetState(PlayerState::End);
	}


	if (UEngineInput::IsPress(VK_LEFT) && UEngineInput::IsPress(VK_RIGHT))
	{
		return;
	}

	if (true == UEngineInput::IsPress(VK_LEFT) || true == UEngineInput::IsPress(VK_RIGHT))
	{
		SetState(PlayerState::Run);
		return;
	}

	if (true == UEngineInput::IsUp(VK_LEFT) || true == UEngineInput::IsUp(VK_RIGHT))
	{
		SetState(PlayerState::Run);
		return;
	}

	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping)
	{
		SetState(PlayerState::Jump);
		return;
	}

}

void Mario::Run(float _DeltaTime)
{
	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping)
	{
		SetState(PlayerState::Jump);
		return;
	}

	if ((UEngineInput::IsFree(VK_LEFT) && UEngineInput::IsFree(VK_RIGHT)) || UEngineInput::IsPress(VK_LEFT) && UEngineInput::IsPress(VK_RIGHT))
	{
		if (CurSpeedDir == 0)
		{
			SetState(PlayerState::Idle);
			return;
		}
		DecreaseSpeed(_DeltaTime, BreakAccelX);
		MoveResult(_DeltaTime);
		return;
	}

	if ((CurSpeed.X) > 300)
	{
		if (UEngineInput::IsPress(VK_LEFT) && CurSpeedDir == 1)
		{
			SetState(PlayerState::DirChange);
			return;
		}
		if (UEngineInput::IsPress(VK_RIGHT) && CurSpeedDir == -1)
		{
			SetState(PlayerState::DirChange);
			return;
		}
	}
	JustRun(_DeltaTime, AccelX);
}

void Mario::Jump(float _DeltaTime)
{
	if (true == UEngineInput::IsUp(VK_SPACE) && CurSpeed.Y < 0.f)
	{
		YSpeed.Y = 0;
		GravitySpeed.Y = 0;
	}

	JustRun(_DeltaTime, AccelX);
	GravityCheck(_DeltaTime);

	if (BreakSpeed.Y == YSpeed.Y && BreakSpeed.Y == GravitySpeed.Y)
	{
		if ((XSpeed.X) > 5)
		{
			SetState(PlayerState::Run);
			Jumping = false;
			return;
		}
		else
		{
			SetState(PlayerState::Idle);
			Jumping = false;
			return;
		}
	}
}


void Mario::IdleStart()
{
	SetAnimation("Idle");
}

void Mario::RunStart()
{
	SetAnimation("Run");
}

void Mario::JumpStart()
{
	Jumping = true;
	YSpeed.Y = JumpPower;
	GravitySpeed.Y = 0;
	AddActorLocation({ 0.f,-5.f });
	SetAnimation("Jump");
}


void Mario::Interactive(float _DeltaTime)
{
	Jumping = true;
	GravityCheck(_DeltaTime);
	JustRun(_DeltaTime, AccelX);

	if (BreakSpeed.Y == YSpeed.Y && BreakSpeed.Y == GravitySpeed.Y) {
		if (abs(XSpeed.X) > 5) {
			SetState(PlayerState::Run);
			Jumping = false;
			return;
		}
		else {
			SetState(PlayerState::Idle);
			Jumping = false;
			return;
		}
	}
}

void Mario::InteractiveStart()
{
	Jumping = true;
	GravitySpeed.Y = 0;
	YSpeed.Y = -300;
	SetAnimation("Jump");
}


void Mario::DieStart()
{
}

void Mario::Die(float _DeltaTime)
{
}


//void Mario::SetAnimation(std::string _Name)
//{
//	std::string Name = GetAniName(_Name);
//	Renderer->ChangeAnimation(Name);
//
//}


void Mario::JustRun(float _DeltaTime, FVector Accel)
{
	if (UEngineInput::IsPress(VK_LEFT) == true) {
		IncreaseSpeed(_DeltaTime, -Accel);
		SetAnimation(CurAnimationName);
		DirCheck();
		MoveResult(_DeltaTime);
		return;
	}

	if (UEngineInput::IsPress(VK_RIGHT) == true) {
		IncreaseSpeed(_DeltaTime, Accel);
		SetAnimation(CurAnimationName);
		MoveResult(_DeltaTime);
		return;
	}
	SetAnimation(CurAnimationName);
	MoveResult(_DeltaTime);
	return;
}




/// ============== 이동 가속 & 감속 ==============

void Mario::IncreaseSpeed(float _DeltaTime, FVector _Fvector)
{
}

void Mario::DecreaseSpeed(float _DeltaTime, FVector _Fvector)
{
}





void Mario::DirCheck()
{
	EActorDir Dir = DirState;

	if (true == UEngineInput::IsPress(VK_LEFT) && false == UEngineInput::IsPress(VK_RIGHT)) {
		DirState = EActorDir::Left;
	}

	else if (true == UEngineInput::IsPress(VK_RIGHT) && false == UEngineInput::IsPress(VK_LEFT)) {
		DirState = EActorDir::Right;
	}

	if (true == Jumping) {
		DirState = Dir;
	}


	if (CurSpeed.X > MinSpeed) {
		CurSpeedDir = 1;
	}
	else if (CurSpeed.X < -MinSpeed) {
		CurSpeedDir = -1;
	}
	else {
		CurSpeedDir = 0;
	}
}

void Mario::DirChange(float _DeltaTime)
{
	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping) {
		SetState(PlayerState::Jump);
		return;
	}

	if (CurSpeedDir == 1) {
		if (UEngineInput::IsPress(VK_LEFT)) {
			DecreaseSpeed(_DeltaTime, BreakAccelX);
			MoveResult(_DeltaTime);
			return;
		}
		else if (UEngineInput::IsPress(VK_RIGHT)) {
			SetState(PlayerState::Run);
			return;
		}
		else if (UEngineInput::IsFree(VK_LEFT)) {
			DecreaseSpeed(_DeltaTime, BreakAccelX);
			MoveResult(_DeltaTime);
			return;
		}
	}

	else if (CurSpeedDir == -1) {
		if (UEngineInput::IsPress(VK_RIGHT)) {
			DecreaseSpeed(_DeltaTime, BreakAccelX);
			MoveResult(_DeltaTime);
			return;
		}
		else if (UEngineInput::IsPress(VK_LEFT)) {
			SetState(PlayerState::Run);
			return;
		}
		else if (UEngineInput::IsFree(VK_RIGHT)) {
			DecreaseSpeed(_DeltaTime, BreakAccelX);
			MoveResult(_DeltaTime);
			return;
		}
	}

	else {
		SetState(PlayerState::Idle);
		return;
	}
}

void Mario::DirChangeStart()
{
}

void Mario::MarioColEffect(float _DeltaTime)
{
}

void Mario::MoveResult(float _DeltaTime)
{
	if (1 == CurSpeedDir) {
		//RightEdgeCheck();
	}
	else if (-1 == CurSpeedDir) {
		//LeftEdgeCheck();
	}

	MarioColEffect(_DeltaTime);

	CurSpeed = BreakSpeed;
	CurSpeed += XSpeed;
	CurSpeed += YSpeed;
	CurSpeed += GravitySpeed;


	AddActorLocation(CurSpeed * _DeltaTime);

	//if (ContentsHelper::BottomCheck(GetActorLocation() + FVector{ 0,-1 })) {
	//	AddActorLocation(FVector{ 0.f,-1.f });
	//}
	MoveLastCamera();

}




//void Mario::MoveLastCameraVector(float _DeltaTime)
//{
//	// 카메라는 x축으로만 움직이게 해본다.
//	GetWorld()->AddCameraPos(MoveVector * _DeltaTime);
//	AddActorLocation(LastMoveVector * _DeltaTime);
//	// 맵에서 원점(0, 0) 이하(마이너스)로 못나가게 하는 코드가
//	// Map 클래스 혹은 PlayLevel에 구현되어있다.
//}







