#include "Collisions.h"

namespace Alchemist {
	// Utility
	float distance(const glm::vec2& first, const glm::vec2& second) {}

	// Collision Detection
	bool check_AABBs(const AABB& first, const AABB& second) {
		bool overlapping_on_x = second.right_up.x > first.left_bottom.x
			|| second.left_bottom.x < first.right_up.x;
		bool overlapping_on_y = second.right_up.y > first.left_bottom.y
			|| second.left_bottom.y < first.right_up.y;

		if (overlapping_on_x && overlapping_on_y)
			return true;
		return false;
	}

	bool check_circles(const Circle& first, const Circle& second) {
		// Check if circles intercept, so check if the distance of their centers is
		// less than the sum of their radi
		float distance_of_centers_squared = (first.center.x - second.center.x) * (first.center.x - second.center.x)
			+ (first.center.y - second.center.y) * (first.center.y - second.center.y);

		float sum_of_radi_squared = (first.radius + second.radius) * (first.radius + second.radius);
		if (distance_of_centers_squared < sum_of_radi_squared)
			return true;
		return false;
	}

	// Collision Resolution
}