#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore\EngineResourcesManager.h>
#include "ContentsHelper.h"


// Ό³Έν :
class Physics : public AActor
{
public:
	// constrcuter destructer
	Physics();
	virtual ~Physics();

	// delete Function
	Physics(const Physics& _Other) = delete;
	Physics(Physics&& _Other) noexcept = delete;
	Physics& operator=(const Physics& _Other) = delete;
	Physics& operator=(Physics&& _Other) noexcept = delete;

protected:
	void AnimationAuto(UImageRenderer* _Renderer, std::string _Name, int _Start = 0, int _End = 0, float _Time = 0.1f, bool _Routine = true);
	virtual std::string GetAnimationName(std::string _Name);
	virtual void SetAnimation(std::string _Name);

	virtual bool GravityCheck(float _DeltaTime);
	virtual void ResultMove(float _DeltaTime);

	virtual void StateUpdate(float _DeltaTime) {};

	//virtual void BoxCollisionEvent(BlockState _BlockState) {};


	std::string CurAnimationName = "";
	UImageRenderer* Renderer = nullptr;
	UCollision* BodyCollision = nullptr;
	EActorDir DirState = EActorDir::Right;

	virtual void BeginPlay() = 0;
	virtual void Tick(float _DeltaTime) = 0;
	bool NoDir = false;
	bool IsCollision = false;

	FVector CurSpeed = { 0,0,0,0 };
	FVector SpeedX = { 0,0,0,0 };
	FVector SpeedY = { 0,0,0,0 };
	FVector GravitySpeed = { 0,0,0,0 };
	const FVector StopSpeed = { 0,0,0,0 };


	UImageRenderer* ScoreRenderer = nullptr;
	FVector ScoreRendererLocation = FVector::Zero;


private:



};

