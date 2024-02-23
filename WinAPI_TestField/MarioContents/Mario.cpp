#include "Mario.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineBase/EngineDebug.h>
#include "ContentsHelper.h"
#include <string>
#include <vector>
#include <EngineCore/Collision.h>

/// ============== 이동에서 중력, 카메라, 가속도 관련 기능 ==============

void Mario::AddMoveVector(const FVector& _DirDelta)
{
	MoveVector += _DirDelta * MoveAcc;
}

void Mario::CalMoveVector(float _DeltaTime)
{
	FVector CheckPos = GetActorLocation();
	switch (DirState)
	{
	case EActorDir::Left:
		CheckPos.X += 35;
		break;
	case EActorDir::Right:
		CheckPos.X += 35;
		break;
	default:
		break;
	}
	CheckPos.Y -= 15;
	Color8Bit Color = ContentsHelper::ColMapImage->GetColor(CheckPos.iX(), CheckPos.iY(), Color8Bit::MagentaA);

	if (Color == Color8Bit(255, 0, 255, 0))
	{
		MoveVector = FVector::Zero;
	}

	if (true == UEngineInput::IsFree(VK_LEFT) && true == UEngineInput::IsFree(VK_RIGHT))
	{
		if (0.001 <= MoveVector.Size2D())
		{
			MoveVector += (-MoveVector.Normalize2DReturn()) * _DeltaTime * MoveAcc;
		}
		else
		{
			MoveVector = float4::Zero;
		}
	}

	if (MoveMaxSpeed <= MoveVector.Size2D())
	{
		MoveVector = MoveVector.Normalize2DReturn() * MoveMaxSpeed;
	}
}

void Mario::CalJumpVector(float _DeltaTime)
{

}

void Mario::CalGravityVector(float _DeltaTime)
{
	GravityVector += GravityAcc * _DeltaTime;
	Color8Bit Color = ContentsHelper::ColMapImage->GetColor(GetActorLocation().iX(), GetActorLocation().iY(), Color8Bit::MagentaA);
	if (Color == Color8Bit(255, 0, 255, 0))
	{
		GravityVector = FVector::Zero;
	}
}


void Mario::CalLastMoveVector(float _DeltaTime)
{
	// 제로로 만들어서 초기화 시킨다.
	LastMoveVector = FVector::Zero;
	LastMoveVector = LastMoveVector + MoveVector;
	LastMoveVector = LastMoveVector + JumpVector;
	LastMoveVector = LastMoveVector + GravityVector;
	LastMoveVector + JumpVector;
}


void Mario::MoveUpdate(float _DeltaTime)
{
	// 이동을 하다가 마리오가 땅이나 오브젝트에 처박힐 수 있는 것을 방지하기 위한 것
	CalMoveVector(_DeltaTime);
	CalGravityVector(_DeltaTime);
	CalLastMoveVector(_DeltaTime);
	MoveLastCameraVector(_DeltaTime);
}

/// ============== 이동에서 중력, 가속도 관련 기능 ==============


/// ========= 마리오 이미지 셋, 애니메이션, 스테이트 =========

Mario* Mario::ItsMeMario = nullptr;

Mario* Mario::GetItsMeMario()
{
	return ItsMeMario;
}

Mario::Mario()
{
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
		Renderer->SetImage("Player_Right.png");
		Renderer->SetTransform({ {0,0}, {256, 256} });
		Renderer->CreateAnimation("Idle_Right", "Player_Right.png", 0, 0, 0.45f, true);
		Renderer->CreateAnimation("Idle_Left", "Player_Left.png", 0, 0, 0.45f, true);

		Renderer->CreateAnimation("Run_Right", "Player_Right.png", { 1, 2, 3 }, 0.1f, true);
		Renderer->CreateAnimation("Run_Left", "Player_Left.png", { 1, 2, 3 }, 0.1f, true);

		Renderer->CreateAnimation("Jump_Right", "Player_Right.png", 5, 5, 0.1f, true);
		Renderer->CreateAnimation("Jump_Left", "Player_Left.png", 5, 5, 0.1f, true);

		Renderer->ChangeAnimation("Idle_Right");
	}

	// 충돌(Collision)이라는 시점을 따로둔다.
	{
		BodyCollision = CreateCollision(MarioCollisionOrder::Player);
		BodyCollision->SetScale({ 10, 100 });
		BodyCollision->SetColType(ECollisionType::CirCle);
	}


	StateChange(PlayerState::Idle);
}


void Mario::Tick(float _DeltaTime)
{
	AActor::Tick(_DeltaTime);

	StateUpdate(_DeltaTime);
}

void Mario::DirCheck()
{
	EActorDir Dir = DirState;
	if (UEngineInput::IsPress(VK_LEFT))
	{
		Dir = EActorDir::Left;
	}
	if (UEngineInput::IsPress(VK_RIGHT))
	{
		Dir = EActorDir::Right;
	}

	if (Dir != DirState)
	{
		DirState = Dir;
		std::string Name = GetAnimationName(CurAnimationName);
		// Renderer->ChangeAnimation(Name, true);
		Renderer->ChangeAnimation(Name, true, Renderer->GetCurAnimationFrame(), Renderer->GetCurAnimationTime());
	}
}


std::string Mario::GetAnimationName(std::string _Name)
{
	std::string DirName = "";

	switch (DirState)
	{
	case EActorDir::Left:
		DirName = "_Left";
		break;
	case EActorDir::Right:
		DirName = "_Right";
		break;
	default:
		break;
	}

	CurAnimationName = _Name;

	return _Name + DirName;
}

void Mario::StateChange(PlayerState _State)
{
	// 이전 상태와 지금의 상태가 같지 않다
	// 어떤 상태 변화? : 이전에는 move, 현재는 Idle
	if (State != _State)
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
		default:
			break;
		}
	}

	State = _State;
}


// ====== 카메라 ======

void Mario::MoveLastCameraVector(float _DeltaTime)
{
	// 카메라는 x축으로만 움직이게 해본다.

	GetWorld()->AddCameraPos(MoveVector * _DeltaTime);

	AddActorLocation(LastMoveVector * _DeltaTime);

	// 
	// 맵에서 원점(0, 0) 이하(마이너스)로 못나가게 하는 코드가
	// Map 클래스 혹은 PlayLevel에 구현되어있다.
}


//void Mario::MoveLastCameraVector(float _DeltaTime)
//{
//	// 카메라는 x축으로만 움직이게 해본다.
//	GetWorld()->AddCameraPos(MoveVector * _DeltaTime);
//	AddActorLocation(LastMoveVector * _DeltaTime);
//	// 맵에서 원점(0, 0) 이하(마이너스)로 못나가게 하는 코드가
//	// Map 클래스 혹은 PlayLevel에 구현되어있다.
//}





void Mario::StateUpdate(float _DeltaTime)
{
	switch (State)
	{
	case PlayerState::CameraFreeMove:
		CameraFreeMove(_DeltaTime);
		break;
	case PlayerState::FreeMove:
		FreeMove(_DeltaTime);
		break;
	case PlayerState::Idle:
		Idle(_DeltaTime);
		break;
	case PlayerState::Run:
		Run(_DeltaTime);
		break;
	case PlayerState::Jump:
		Jump(_DeltaTime);
		break;
	default:
		break;
	}

}

void Mario::CameraFreeMove(float _DeltaTime)
{
	if (UEngineInput::IsPress(VK_LEFT))
	{
		GetWorld()->AddCameraPos(FVector::Left * _DeltaTime * 500.0f);
		// AddActorLocation(FVector::Left * _DeltaTime * 500.0f);
	}

	if (UEngineInput::IsPress(VK_RIGHT))
	{
		//                             2        *  시간      * 500
		GetWorld()->AddCameraPos(FVector::Right * _DeltaTime * 500.0f);
	}

	if (UEngineInput::IsPress(VK_UP))
	{
		GetWorld()->AddCameraPos(FVector::Up * _DeltaTime * 500.0f);
		// AddActorLocation(FVector::Up * _DeltaTime * 500.0f);
	}

	if (UEngineInput::IsPress(VK_DOWN))
	{
		GetWorld()->AddCameraPos(FVector::Down * _DeltaTime * 500.0f);
		// AddActorLocation(FVector::Down * _DeltaTime * 500.0f);
	}

	if (UEngineInput::IsDown('2'))
	{
		StateChange(PlayerState::Idle);
	}
}


void Mario::FreeMove(float _DeltaTime)
{
	FVector FreeMove = FVector::Zero;

	if (UEngineInput::IsPress(VK_LEFT))
	{
		FreeMove += FVector::Left * _DeltaTime * FreeMoveSpeed;
	}

	if (UEngineInput::IsPress(VK_RIGHT))
	{
		FreeMove += FVector::Right * _DeltaTime * FreeMoveSpeed;
	}

	if (UEngineInput::IsPress(VK_UP))
	{
		FreeMove += FVector::Up * _DeltaTime * FreeMoveSpeed;
	}

	if (UEngineInput::IsPress(VK_DOWN))
	{
		FreeMove += FVector::Down * _DeltaTime * FreeMoveSpeed;
	}

	AddActorLocation(FreeMove * _DeltaTime);
	GetWorld()->AddCameraPos(FreeMove * _DeltaTime);

	if (UEngineInput::IsDown('1'))
	{
		StateChange(PlayerState::Idle);
	}
}

// ===== 카메라 ======


// ====== 키 입력으로 인한 마리오의 움직임 =======

void Mario::Idle(float _DeltaTime)
{
	if (true == Renderer->IsCurAnimationEnd())
	{
		int a = 0;
	}

	// 왼쪽도, 오른쪽도 안가고 있고,
	// 여기서는 가만히 있을때만 어떻게 할지 신경쓰면 된다.
	if (true == UEngineInput::IsDown('1'))
	{
		StateChange(PlayerState::FreeMove);
		return;
	}

	if (true == UEngineInput::IsDown('2'))
	{
		StateChange(PlayerState::CameraFreeMove);
		return;
	}


	if (
		true == UEngineInput::IsPress(VK_LEFT) ||
		true == UEngineInput::IsPress(VK_RIGHT)
		)
	{
		StateChange(PlayerState::Run);
		return;
	}

	if (true == UEngineInput::IsDown(VK_SPACE))
	{
		StateChange(PlayerState::Jump);
		return;
	}

	MoveUpdate(_DeltaTime);

}

void Mario::Jump(float _DeltaTime)
{
	if (UEngineInput::IsPress(VK_LEFT))
	{
		AddMoveVector(FVector::Left * _DeltaTime);
	}

	if (UEngineInput::IsPress(VK_RIGHT))
	{
		AddMoveVector(FVector::Right * _DeltaTime);
	}

	MoveUpdate(_DeltaTime);

	Color8Bit Color = ContentsHelper::ColMapImage->GetColor(GetActorLocation().iX(), GetActorLocation().iY(), Color8Bit::MagentaA);
	if (Color == Color8Bit(255, 0, 255, 0))
	{
		JumpVector = FVector::Zero;
		StateChange(PlayerState::Idle);
		return;
	}
}

void Mario::Run(float _DeltaTime)
{
	DirCheck();

	if (true == UEngineInput::IsFree(VK_LEFT) && UEngineInput::IsFree(VK_RIGHT))
	{
		StateChange(PlayerState::Idle);
		return;
	}

	if (UEngineInput::IsPress(VK_LEFT))
	{
		AddMoveVector(FVector::Left * _DeltaTime);
	}

	if (UEngineInput::IsPress(VK_RIGHT))
	{
		AddMoveVector(FVector::Right * _DeltaTime);
	}

	if (true == UEngineInput::IsDown(VK_SPACE))
	{
		StateChange(PlayerState::Jump);
		return;
	}


	MoveUpdate(_DeltaTime);
}

void Mario::IdleStart()
{
	Renderer->ChangeAnimation(GetAnimationName("Idle"));
	DirCheck();
}

void Mario::RunStart()
{
	Renderer->ChangeAnimation(GetAnimationName("Run"));
	DirCheck();
}

void Mario::JumpStart()
{
	JumpVector = JumpPower;
	Renderer->ChangeAnimation(GetAnimationName("Jump"));
	DirCheck();
}


