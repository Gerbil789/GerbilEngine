#include "enginepch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
	{
		using namespace glm;
		using T = float;

		mat4 LocalTransform = transform;

		// Normalize the matrix.
		if (epsilonEqual(LocalTransform[3][3], static_cast<T>(0), epsilon<T>()))
			return false;

		// First, isolate perspective. This is the messiest.
		if (epsilonNotEqual(LocalTransform[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalTransform[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalTransform[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalTransform[0][3] = LocalTransform[1][3] = LocalTransform[2][3] = static_cast<T>(0);
			LocalTransform[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		position = vec3(LocalTransform[3]);
		LocalTransform[3] = vec4(0, 0, 0, LocalTransform[3].w);

		vec3 row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; i++)
			for (length_t j = 0; j < 3; j++)
				row[i][j] = LocalTransform[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(row[0]);
		row[0] = detail::scale(row[0], static_cast<T>(1));
		scale.y = length(row[1]);
		row[1] = detail::scale(row[1], static_cast<T>(1));
		scale.z = length(row[2]);
		row[2] = detail::scale(row[2], static_cast<T>(1));


		rotation.y = asin(-row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(row[1][2], row[2][2]);
			rotation.z = atan2(row[0][1], row[0][0]);
		}
		else {
			rotation.x = atan2(-row[2][0], row[1][1]);
			rotation.z = 0;
		}

		return true;
	}
}