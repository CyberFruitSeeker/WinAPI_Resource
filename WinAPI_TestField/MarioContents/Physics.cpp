#include "Physics.h"
#include "Mario.h"
#include <EngineCore/EngineResourcesManager.h>
#include <MarioContents/ContentsHelper.h>


Physics::Physics()
{
}

Physics::~Physics()
{
}


void Physics::Tick(float _DeltaTime)
{

}


/// <summary>
/// 
/// </summary>
/// <param name="_Renderer"></param>
/// <param name="_Name"></param>
/// <param name="_Start"></param>
/// <param name="_End"></param>
/// <param name="_Time">0.1f</param>
/// <param name="_DoubleWay">true</param>
/// <param name="_Routine">true</param>
void Physics::AnimationAuto(UImageRenderer* _Renderer, std::string _Name, int _Start, int _End, float _Time, bool _Routine)
{
	std::string CurName = GetName();
	if (NoDir) {
		_Renderer->CreateAnimation(_Name, CurName + ".PNG", _Start, _End, _Time, _Routine);
		return;
	}
	_Renderer->CreateAnimation(_Name + "_Right", CurName + "_Right.PNG", _Start, _End, _Time, _Routine);
	_Renderer->CreateAnimation(_Name + "_Left", CurName + "_Left.PNG", _Start, _End, _Time, _Routine);
	return;
}


std::string Physics::GetAnimationName(std::string _Name)
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

	if (NoDir) {
		DirName = "";
	}

	CurAnimationName = _Name;

	return _Name + DirName;
}

void Physics::SetAnimation(std::string _Name)
{
	std::string Name = GetAnimationName(_Name);
	Renderer->ChangeAnimation(Name);
}

bool Physics::GravityCheck(float _DeltaTime)
{

	FVector CurLocation = GetActorLocation();
	GravitySpeed += ContentsHelper::Gravity * _DeltaTime;

	Color8Bit Color_Right = ContentsHelper::ColMapImage->GetColor(GetActorLocation().iX() + 28, GetActorLocation().iY(), Color8Bit::MagentaA);
	Color8Bit Color_Left = ContentsHelper::ColMapImage->GetColor(GetActorLocation().iX() - 28, GetActorLocation().iY(), Color8Bit::MagentaA);

	if (Color_Right == Color8Bit(255, 0, 255, 0) || Color_Left == Color8Bit(255, 0, 255, 0) || IsCollision == true)
	{
		GravitySpeed = StopSpeed;
		SpeedY = StopSpeed;
		return false;
	}

	return true;
}


void Physics::ResultMove(float _DeltaTime)
{
	CurSpeed = StopSpeed;
	CurSpeed += SpeedX;
	CurSpeed += SpeedY;
	CurSpeed += GravitySpeed;
	AddActorLocation(CurSpeed * _DeltaTime);
}




