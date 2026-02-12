#pragma once

#include "Engine/Script/Script.h"
#include "Engine/Graphics/Texture.h"

class PlayerController : public Engine::Script
{
public:
  float MoveSpeed = 10.0f;
	float RotateSpeed = 50.0f;
	bool Rotate = true;

	Engine::Texture2D* TestTexture = nullptr;

	void OnCreate() override;

	void OnUpdate(float delta) override;

private:
	void ProcessInput(float delta);
};