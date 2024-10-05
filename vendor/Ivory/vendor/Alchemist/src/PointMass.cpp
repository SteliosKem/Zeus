
#include "PointMass.h"
#include <iostream>

namespace Alchemist {
	void PointMass2D::integrate(float dt) {
		glm::vec2 final_acceleration = m_acceleration + m_force_accumulator * m_mass_inverse;
		// Update velocity
		m_velocity += final_acceleration * dt;
		// Scale velocity down depending on damping factor
		m_velocity *= powf(m_damping, dt);

		// Update position
		m_position += m_velocity * dt;
		m_shape.center = m_position;

		// Zero out forces
		clear_force_accumulator();
	}

	void ForceRegistry::update_forces(float dt) {
		for (const auto& i : registrations) {
			i.force_generator->update_force(i.point_mass, dt);
		}
	}

	void PointMassGravity::update_force(PointMass2D* point_mass, float dt) {
		point_mass->add_force(m_gravity * point_mass->get_mass());
	}

	void SimpleForce::update_force(PointMass2D* point_mass, float dt) {
		point_mass->add_force(m_force * point_mass->get_mass());
	}

	// Springs

	void PointMassSpring::update_force(PointMass2D* point_mass, float dt) {
		// Get other attached object position
		const glm::vec2& other_object_position = point_mass->get_position();
		// Distance between the two attached objects as a vector
		glm::vec2 total_distance_vector = other_object_position - m_attached_object->get_position();
		m_spring_length = total_distance_vector;	// Only used to get length for a renderer

		// ----
		// Here the spring's rest length is used to make a vector with the length of the 
		// spring's rest length and the direction of the total_distance vector
		glm::vec2 rest_length_vector;
		if (total_distance_vector.x != 0 || total_distance_vector.y != 0)
			rest_length_vector = (total_distance_vector * (float)1 / sqrtf(total_distance_vector.x * total_distance_vector.x
				+ total_distance_vector.y * total_distance_vector.y)) * m_spring_rest_length;
		else rest_length_vector = { 0, 0 };
		// ----

		// Use the formula F = -k * x, where x is the distance between the spring's rest point and
		// The other object's position
		glm::vec2 force = -m_spring_constant * (total_distance_vector - rest_length_vector);
		point_mass->add_force(force);
		if(m_first_object_affected)
			m_attached_object->add_force(-force);
	}

	void AnchoredPointMassSpring::update_force(PointMass2D* point_mass, float dt) {
		// Get other attached object position
		const glm::vec2& other_object_position = point_mass->get_position();
		// Distance between the two attached objects as a vector
		glm::vec2 total_distance_vector = other_object_position - m_position;
		m_spring_length = total_distance_vector;	// Only used to get length for a renderer

		// ----
		// Here the spring's rest length is used to make a vector with the length of the 
		// spring's rest length and the direction of the total_distance vector
		glm::vec2 rest_length_vector;
		if (total_distance_vector.x != 0 || total_distance_vector.y != 0)
			rest_length_vector = (total_distance_vector * (float)1 / sqrtf(total_distance_vector.x * total_distance_vector.x
				+ total_distance_vector.y * total_distance_vector.y)) * m_spring_rest_length;
		else rest_length_vector = { 0, 0 };
		// ----
		
		// Use the formula F = -k * x, where x is the distance between the spring's rest point and
		// The other object's position
		glm::vec2 force = -m_spring_constant * (total_distance_vector - rest_length_vector);
		point_mass->add_force(force);
	}

	void resolve_elastic_collision_circle(PointMass2D& first, PointMass2D& second) {
		/*glm::vec2 relative_velocity = second.get_velocity() - first.get_velocity();
		glm::vec2 distance_from_centers_vec = second.get_position() - first.get_position();
		glm::vec2 normal = distance_from_centers_vec /
			sqrtf(distance_from_centers_vec.x * distance_from_centers_vec.x + distance_from_centers_vec.y * distance_from_centers_vec.y);
		float normal_velocity = 
		*/
		float m1 = first.get_mass();
		float m2 = second.get_mass();
		glm::vec2& v1 = first.get_velocity();
		glm::vec2& v2 = second.get_velocity();

		glm::vec2& v1_final = ((2 * m2) / (m1 + m2)) * v2 + ((m1 - m2) / (m1 + m2)) * v1;
		glm::vec2& v2_final = ((2 * m1) / (m1 + m2)) * v1 + ((m2 - m1) / (m1 + m2)) * v2;
		first.set_velocity(v1_final);
		second.set_velocity(v2_final);
	}

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

	bool check_collision(const PointMass2D& first, const PointMass2D& second) {
		// Check if circles intercept, so check if the distance of their centers is
		// less than the sum of their radi
		float distance_of_centers_squared = (first.get_position().x - second.get_position().x) * (first.get_position().x - second.get_position().x)
			+ (first.get_position().y - second.get_position().y) * (first.get_position().y - second.get_position().y);

		float sum_of_radi_squared = (first.get_shape().radius + second.get_shape().radius) * (first.get_shape().radius + second.get_shape().radius);
		if (distance_of_centers_squared < sum_of_radi_squared)
			return true;
		return false;
	}
}