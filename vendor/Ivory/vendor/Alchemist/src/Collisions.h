#pragma once
#include <glm/glm.hpp>

namespace Alchemist {
	struct AABB {
		glm::vec2 left_bottom;
		glm::vec2 right_up;
	};

	struct Circle {
		float radius;
		glm::vec2 center;
	};

	// Utility
	float distance(const glm::vec2& first, const glm::vec2& second);

	// Collision Detection
	bool check_AABBs(const AABB& first, const AABB& second);
	bool check_circles(const Circle& first, const Circle& second);

	// Collision Resolution
}