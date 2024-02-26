#include "Mario.h"
#include <EnginePlatform/EngineInput.h>
#include <EngineBase/EngineDebug.h>
#include <EngineCore/Collision.h>
#include <string>
#include <vector>
#include <list>
#include "Physics.h"
#include "ContentsHelper.h"

MarioClass Mario::MyMarioClass = MarioClass::Small;
FVector Mario::PlayerLocation = {};

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
	SetName("Mario");
	Renderer = CreateImageRenderer(MarioRenderOrder::Player);
	Renderer->SetImage("Mario_Right.png");
	Renderer->SetTransform({ {0,0}, {256, 256} });

	AnimationAuto(Renderer, "Idle", 0, 0);
	AnimationAuto(Renderer, "Move", 1, 3);
	AnimationAuto(Renderer, "Jump", 5, 5);
	AnimationAuto(Renderer, "DirChange", 4, 4);
	AnimationAuto(Renderer, "Dead", 6, 6);
	AnimationAuto(Renderer, "End", 7, 7);

	//AnimationAuto(Renderer, "Bigger", 18, 19, 0.1f);
	//AnimationAuto(Renderer, "Big_Smaller", 18, 19, 0.1f);
	//AnimationAuto(Renderer, "Smaller", 18, 19, 0.1f);
	//AnimationAuto(Renderer, "Big_Fire", 32, 33, 0.1f);
	//AnimationAuto(Renderer, "Fire", 32, 33, 0.1f);
	//AnimationAuto(Renderer, "Fire_Smaller", 32, 33, 0.1f);

	//MushRoomRenderer = CreateImageRenderer(MarioRenderOrder::Player);
	//MushRoomRenderer->SetImage("Mario_Right.png");
	//MushRoomRenderer->SetTransform({ {0,0}, {256, 256} });


	AnimationAuto(Renderer, "Big_Idle", 9, 9);
	AnimationAuto(Renderer, "Big_Move", 10, 12);
	AnimationAuto(Renderer, "Big_DirChange", 13, 13);
	AnimationAuto(Renderer, "Big_Jump", 14, 14);

	AnimationAuto(Renderer, "Fire_Idle", 20, 20);
	AnimationAuto(Renderer, "Fire_Move", 21, 23);
	AnimationAuto(Renderer, "Fire_DirChange", 24, 24);
	AnimationAuto(Renderer, "Fire_Jump", 25, 25);





	BodyCollision = CreateCollision(MarioCollisionOrder::Player);
	BodyCollision->SetColType(ECollisionType::Rect);
	BodyCollision->SetTransform({ { 0,-32 }, { 64, 64 } });

	SetAnimation("Idle");
	SetState(PlayerState::Idle);

}

void Mario::Tick(float _DeltaTime)
{
	Physics::Tick(_DeltaTime);

	PlayerLocation = GetActorLocation();

	/*if (MyMarioClass == MarioClass::Fire && UEngineInput::IsDown('Z')) {
		AFire* Fire = GetWorld()->SpawnActor<AFire>(MarioRenderOrder::Fire);
		Fire->SetActorLocation(GetActorLocation());
		Fire->SetDirState(DirState);
	}*/

	if (ChangeTime >= 0.f) {
		ChangeTime -= _DeltaTime;
	}
	if (GodTime >= 0.f) {
		GodTime -= _DeltaTime;
	}

	StateUpdate(_DeltaTime);

}

void Mario::SetActorCameraPos()
{
	FVector CurPos = GetActorLocation();
	FVector CurCameraPos = GetWorld()->GetCameraPos();
	float WindowCenter = GEngine->MainWindow.GetWindowScale().hX();
	if (CurPos.X > WindowCenter + CurCameraPos.X) {
		GetWorld()->SetCameraPos({ CurPos.X - WindowCenter,CurCameraPos.Y });
	}
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
	case PlayerState::Move:
		GravityCheck(_DeltaTime);
		Move(_DeltaTime);
		break;
	case PlayerState::Jump:
		Jump(_DeltaTime);
		break;
	case PlayerState::DirChange:
		GravityCheck(_DeltaTime);
		DirChange(_DeltaTime);
		break;
	case PlayerState::Interactive:
		Interactive(_DeltaTime);
		break;
	case PlayerState::Dead:
		Dead(_DeltaTime);
		break;
	case PlayerState::End:
		End(_DeltaTime);
		break;
	case PlayerState::EndMove:
		EndMove(_DeltaTime);
		break;
	case PlayerState::Changing:
		Changing(_DeltaTime);
		break;
	default:
		break;
	}

}



void Mario::AddSpeed(float _DeltaTime, FVector _FVector) {

	float NextFloatX = abs(CurSpeed.X + (_FVector.X * _DeltaTime));
	if (MaxSpeedX >= NextFloatX) {
		SpeedX.X += _FVector.X * _DeltaTime;
	}

	float NextFloatY = abs(CurSpeed.Y + (_FVector.Y * _DeltaTime));
	if (MaxSpeedY >= NextFloatY) {
		SpeedY.Y += _FVector.Y * _DeltaTime;
	}
}

void Mario::SubtractSpeed(float _DeltaTime, FVector _FVector)
{
	if (CurSpeedDir == -1) {
		SpeedX += (_FVector * _DeltaTime);
	}
	else if (CurSpeedDir == 1) {
		SpeedX -= (_FVector * _DeltaTime);
	}
}



void Mario::SetState(PlayerState _State)
{
	if (_State == PlayerState::Dead) {
		if (GodTime >= 0.f) {
			return;
		}
	}

	if (State != _State || _State == PlayerState::Interactive)
	{
		switch (_State)
		{
		case PlayerState::Idle:
			IdleStart();
			break;
		case PlayerState::Move:
			MoveStart();
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
		case PlayerState::Dead:
			DeadStart();
			break;
		case PlayerState::End:
			if (State == PlayerState::EndMove) {
				return;
			}
			EndStart();
			break;
		case PlayerState::EndMove:
			EndMoveStart();
			break;
		case PlayerState::Changing:
			ChangingStart();
			break;
		default:
			break;
		}
	}
	State = _State;
}

void Mario::SetMarioClassState(MarioClass _MarioClass)
{
	if (MyMarioClass == _MarioClass) {
		return;
	}
	ChangeTime = 1.f;
	switch (_MarioClass)
	{
	case MarioClass::Small:
		GodTime = 2.f;
		SetAnimation("Smaller");
		BodyCollision->SetTransform({ { 0,-32 }, { 64, 64 } });
		break;
	case MarioClass::Big:
		SetAnimation("Bigger");
		BodyCollision->SetTransform({ { 0,-64 }, { 64, 128 } });
		break;
	case MarioClass::Fire:
		SetAnimation("Fire");
		BodyCollision->SetTransform({ { 0,-64 }, { 64, 128 } });
		break;
	default:
		break;
	}
	MyMarioClass = _MarioClass;
	SetState(PlayerState::Changing);
}

void Mario::IdleStart()
{
	SetAnimation("Idle");
}

void Mario::MoveStart()
{
	SetAnimation("Move");
}

void Mario::JumpStart()
{
	if (PrevState == PlayerState::Jump) {
		Jumping = true;
		SpeedY.Y = 0;
		GravitySpeed.Y = 0;
		AddActorLocation({ 0.f,-4.f });
		SetAnimation("Jump");
		PrevState = PlayerState::None;
	}
	else {
		Jumping = true;
		SpeedY.Y = JumpPower;
		GravitySpeed.Y = 0;
		AddActorLocation({ 0.f,-4.f });
		SetAnimation("Jump");
	}
}
void Mario::DirChangeStart()
{
	SetAnimation("DirChange");
}


void Mario::InteractiveStart()
{
	Jumping = true;
	GravitySpeed.Y = 0;
	SpeedY.Y = -500;
	SetAnimation("Jump");
}

void Mario::Idle(float _DeltaTime)
{

	ResultMove(_DeltaTime);


	if (0 == CurSpeedDir) {
		SpeedX.X = 0;
	}

	if (UEngineInput::IsPress(VK_CONTROL)) {
		SetState(PlayerState::End);
	}

	if (UEngineInput::IsPress(VK_LEFT) && UEngineInput::IsPress(VK_RIGHT)) {
		return;
	}

	if (true == UEngineInput::IsPress(VK_LEFT) || true == UEngineInput::IsPress(VK_RIGHT)) {
		SetState(PlayerState::Move);
		return;
	}

	if (true == UEngineInput::IsUp(VK_LEFT) || true == UEngineInput::IsUp(VK_RIGHT)) {
		SetState(PlayerState::Move);
		return;
	}



	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping) {
		SetState(PlayerState::Jump);
		return;
	}


}

void Mario::DeadStart()
{
	GravitySpeed.Y = 0.f;
	SpeedX.X = 0.f;
	SpeedY.Y = -500.f;
	SetAnimation("Dead");
	BodyCollision->Destroy();

}

void Mario::EndStart()
{
	SetAnimation("End");
}

void Mario::EndMoveStart()
{
	DirState = EActorDir::Left;
	SetAnimation("End");
	float CurPosX = GetActorLocation().X + 64.f;
	float CurPosY = GetActorLocation().Y;
	SetActorLocation({ CurPosX , CurPosY });
}

void Mario::Move(float _DeltaTime)
{
	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping) {
		SetState(PlayerState::Jump);
		return;
	}

	GravityCheck(_DeltaTime);

	if ((UEngineInput::IsFree(VK_LEFT) && UEngineInput::IsFree(VK_RIGHT)) ||
		UEngineInput::IsPress(VK_LEFT) && UEngineInput::IsPress(VK_RIGHT))
	{
		if (CurSpeedDir == 0) {
			SetState(PlayerState::Idle);
			return;
		}
		SubtractSpeed(_DeltaTime, StopAccelerateX);
		ResultMove(_DeltaTime);
		return;
	}


	if (abs(CurSpeed.X) > 300) {
		if (UEngineInput::IsPress(VK_LEFT) && CurSpeedDir == 1) {
			SetState(PlayerState::DirChange);
			return;
		}
		if (UEngineInput::IsPress(VK_RIGHT) && CurSpeedDir == -1) {
			SetState(PlayerState::DirChange);
			return;
		}
	}
	MoveFun(_DeltaTime, AccelerateX);
}

void Mario::Dead(float _DeltaTime) {

	if (DeadTime < 0.f) {
		GravitySpeed += ContentsHelper::Gravity * _DeltaTime;
		float Y = SpeedY.Y + GravitySpeed.Y;
		AddActorLocation(FVector{ 0.f,Y * _DeltaTime });
		Destroy(3.f);
	}
	else {
		DeadTime -= _DeltaTime;
	}

}

void Mario::End(float _DeltaTime)
{
	float pos = 100.f * _DeltaTime;
	AddActorLocation({ 0.f, pos });

	if (UEngineInput::IsPress(VK_SHIFT)) {
		SetState(PlayerState::EndMove);
	}
}

void Mario::ChangingStart()
{
	PrevState = State;
	ChangeTime = 1.f;
}

void Mario::EndMove(float _DeltaTime)
{
	if (EndTime >= 0) {
		EndTime -= _DeltaTime;
		return;
	}
	DirState = EActorDir::Right;
	SetAnimation("Move");
	AddActorLocation({ 100.f * _DeltaTime, 0.f });
}
void Mario::Jump(float _DeltaTime)
{
	if (IsCollision) {
		//return;
	}
	if (true == UEngineInput::IsUp(VK_SPACE) && CurSpeed.Y < 0.f) {
		SpeedY.Y = 0;
		GravitySpeed.Y = 0;
	}

	MoveFun(_DeltaTime, AccelerateX);
	GravityCheck(_DeltaTime);

	if (StopSpeed.Y == SpeedY.Y && StopSpeed.Y == GravitySpeed.Y) {
		if (abs(SpeedX.X) > 5) {
			SetState(PlayerState::Move);
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

void Mario::DirChange(float _DeltaTime)
{
	if (UEngineInput::IsDown(VK_SPACE) && false == Jumping) {
		SetState(PlayerState::Jump);
		return;
	}

	if (CurSpeedDir == 1) {
		if (UEngineInput::IsPress(VK_LEFT)) {
			SubtractSpeed(_DeltaTime, StopAccelerateX);
			ResultMove(_DeltaTime);
			return;
		}
		else if (UEngineInput::IsPress(VK_RIGHT)) {
			SetState(PlayerState::Move);
			return;
		}
		else if (UEngineInput::IsFree(VK_LEFT)) {
			SubtractSpeed(_DeltaTime, StopAccelerateX);
			ResultMove(_DeltaTime);
			return;
		}
	}

	else if (CurSpeedDir == -1) {
		if (UEngineInput::IsPress(VK_RIGHT)) {
			SubtractSpeed(_DeltaTime, StopAccelerateX);
			ResultMove(_DeltaTime);
			return;
		}
		else if (UEngineInput::IsPress(VK_LEFT)) {
			SetState(PlayerState::Move);
			return;
		}
		else if (UEngineInput::IsFree(VK_RIGHT)) {
			SubtractSpeed(_DeltaTime, StopAccelerateX);
			ResultMove(_DeltaTime);
			return;
		}
	}

	else {
		SetState(PlayerState::Idle);
		return;
	}

}


void Mario::SetAnimation(std::string _Name)
{
	std::string Name = GetAnimationName(_Name);
	switch (MyMarioClass)
	{
	case MarioClass::Small:
		break;
	case MarioClass::Big:
		Name = "Big_" + Name;
		break;
	case MarioClass::Fire:
		Name = "Fire_" + Name;
		break;
	default:
		break;
	}

	if (Name._Equal("Big_Bigger_Left") || Name._Equal("Big_Bigger_Right")) {
		return;
	}

	Renderer->ChangeAnimation(Name);
}


void Mario::MoveFun(float _DeltaTime, FVector _FVector)
{
	if (UEngineInput::IsPress(VK_LEFT) == true) {
		AddSpeed(_DeltaTime, -_FVector);
		SetAnimation(CurAnimationName);
		DirCheck();
		ResultMove(_DeltaTime);
		return;
	}

	if (UEngineInput::IsPress(VK_RIGHT) == true) {
		AddSpeed(_DeltaTime, _FVector);
		SetAnimation(CurAnimationName);
		ResultMove(_DeltaTime);
		return;
	}
	SetAnimation(CurAnimationName);
	ResultMove(_DeltaTime);
	return;
}

void Mario::Interactive(float _DeltaTime)
{
	Jumping = true;
	GravityCheck(_DeltaTime);
	MoveFun(_DeltaTime, AccelerateX);

	if (StopSpeed.Y == SpeedY.Y && StopSpeed.Y == GravitySpeed.Y) {
		if (abs(SpeedX.X) > 5) {
			SetState(PlayerState::Move);
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

bool Mario::LeftEdgeCheck()
{

	FVector CurLocation = GetActorLocation();

	if (ContentsHelper::LeftCheck(CurLocation)) {
		AddActorLocation({ 0.5f, 0.f });
		SpeedX.X = 0;
		return false;
	}
	return true;

	int EdgeLocation_Left = static_cast<int>(CurLocation.X - 32.f);
	int EdgeLocation_Top = static_cast<int>(CurLocation.Y - 32.f);
	int EdgeLocation_Bottom = static_cast<int>(CurLocation.Y - 3.f);

	Color8Bit CheckColor_LeftTop = ContentsHelper::ColMapImage->GetColor(EdgeLocation_Left, EdgeLocation_Top, Color8Bit::MagentaA);
	Color8Bit CheckColor_LeftBottom = ContentsHelper::ColMapImage->GetColor(EdgeLocation_Left, EdgeLocation_Bottom - 2, Color8Bit::MagentaA);

	bool FirstCondition = (Color8Bit(255, 0, 255, 0) == CheckColor_LeftBottom);
	bool SecondCondition = (Color8Bit(255, 0, 255, 0) == CheckColor_LeftTop);

	if (FirstCondition || SecondCondition) {
		AddActorLocation({ 0.5f, 0.f });
		SpeedX.X = 0;
		return false;
	}
	return true;
}

bool Mario::RightEdgeCheck()
{

	FVector CurLocation = GetActorLocation();
	if (ContentsHelper::RightCheck(CurLocation)) {
		AddActorLocation({ -0.5f, 0.f });
		SpeedX.X = 0;
		return false;
	}
	return true;



	int EdgeLocation_Right = static_cast<int>(CurLocation.X + 32.f);
	int EdgeLocation_Top = static_cast<int>(CurLocation.Y - 32.f);
	int EdgeLocation_Bottom = static_cast<int>(CurLocation.Y - 3.f);

	Color8Bit CheckColor_RightTop = ContentsHelper::ColMapImage->GetColor(EdgeLocation_Right, EdgeLocation_Top, Color8Bit::MagentaA);
	Color8Bit CheckColor_RightBottom = ContentsHelper::ColMapImage->GetColor(EdgeLocation_Right, EdgeLocation_Bottom - 2, Color8Bit::MagentaA);

	bool FirstCondition = (Color8Bit(255, 0, 255, 0) == CheckColor_RightBottom);
	bool SecondCondition = (Color8Bit(255, 0, 255, 0) == CheckColor_RightTop);

	if (FirstCondition || SecondCondition) {
		AddActorLocation({ -0.5f, 0.f });
		SpeedX.X = 0;
		return false;
	}
	return true;
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

void Mario::MarioCollisionEvent(float _DeltaTime)
{
	//std::vector<UCollision*> Result;
	//IsCollision = false;
	//if (true == BodyCollision->CollisionCheck(MarioCollisionOrder::Block, Result))
	//{
	//	/*UCollision* Collision = Result[0];
	//	BlockBase* Block = static_cast<BlockBase*>(Collision->GetOwner());
	//	FVector BlockVector = Block->GetActorLocation();*/

	//	/*float LeftX = BlockVector.X - 32.f;
	//	float RightX = BlockVector.X + 32.f;
	//	float TopY = BlockVector.Y - 64.f;
	//	float BottomY = BlockVector.Y;*/

	//	FVector CurLocation = GetActorLocation();

	//	/*if (CurLocation.X > LeftX && RightX > CurLocation.X) {
	//		GravitySpeed.Y = 0;
	//		SpeedY.Y = 0;
	//		if (CurLocation.Y > BottomY) {
	//			AddActorLocation(FVector{ 0.f,1.f });
	//			Block->SetBoxState(BlockState::Interactive);
	//		}
	//		else {
	//			IsCollision = true;
	//		}
	//	}

	//	if (CurSpeedDir == 1 && CurLocation.X < LeftX) {
	//		if (CurLocation.Y - 64 > TopY || CurLocation.Y < BottomY) {
	//			SpeedX.X = 0;
	//			if (TopY + 4 < CurLocation.Y) {
	//				AddActorLocation(FVector{ -1.f,0.f });
	//			}
	//		}
	//	}*/
	//	/*if (CurSpeedDir == -1 && CurLocation.X > RightX) {
	//		if (CurLocation.Y - 64 > TopY || CurLocation.Y < BottomY) {
	//			SpeedX.X = 0;
	//			if (TopY + 4 < CurLocation.Y) {
	//				AddActorLocation(FVector{ 1.f,0.f });
	//			}
	//		}
	//	}*/
	//}
}



void Mario::Changing(float _DeltaTime)
{
	if (ChangeTime <= 0) {
		SetState(PrevState);
	}
}

void Mario::MarioChange(bool _Positive)
{
	if (_Positive) {
		switch (MyMarioClass)
		{
		case MarioClass::Small:
			break;
		case MarioClass::Big:
			break;
		case MarioClass::Fire:
			break;
		default:
			break;
		}
	}
	else {
		switch (MyMarioClass)
		{
		case MarioClass::Small:
			break;
		case MarioClass::Big:
			break;
		case MarioClass::Fire:
			break;
		default:
			break;
		}
	}
}


void Mario::ResultMove(float _DeltaTime)
{
	if (1 == CurSpeedDir) {
		RightEdgeCheck();
	}
	else if (-1 == CurSpeedDir) {
		LeftEdgeCheck();
	}

	MarioCollisionEvent(_DeltaTime);

	CurSpeed = StopSpeed;
	CurSpeed += SpeedX;
	CurSpeed += SpeedY;
	CurSpeed += GravitySpeed;


	AddActorLocation(CurSpeed * _DeltaTime);

	if (ContentsHelper::BottomCheck(GetActorLocation() + FVector{ 0,-1 })) {
		AddActorLocation(FVector{ 0.f,-1.f });
	}
	SetActorCameraPos();
}
