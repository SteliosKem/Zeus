
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
}