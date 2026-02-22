#pragma once

#include "Engine/Script/Script.h"

class PlayerController : public Engine::Script
{
	SCRIPT_CLASS(PlayerController)

public:

  PROPERTY(MoveSpeed, "Move Speed")
    float MoveSpeed = 10.0f;

  PROPERTY(RotateSpeed, "Rotate Speed")
    float RotateSpeed = 50.0f;

  PROPERTY(Rotate, "Rotate")
    bool Rotate = true;

  PROPERTY(TestTexture, "TEST")
    Engine::Texture2D* TestTexture = nullptr;

  PROPERTY(clip, "Audio test")
    Engine::AudioClip* clip = nullptr;

	void OnCreate() override;
	void OnUpdate() override;

private:
	void ProcessInput(float delta);
};