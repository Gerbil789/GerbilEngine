#include "RandomMovement.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Time.h"
#include <cmath>

using namespace Engine;

void RandomMovement::OnCreate()
{

}

void RandomMovement::OnUpdate()
{
	auto& transform = Self.GetComponent<TransformComponent>();

	m_ElapsedTime += Time::DeltaTime();

  // Normalize sine output from [-1,1] to [0,1]
  float t = (std::sinf(m_ElapsedTime * m_Speed) + 1.0f) / 2.0f;

  float scale = m_MinScale + t * (m_MaxScale - m_MinScale);

  transform.scale = { scale, scale, scale };
}
