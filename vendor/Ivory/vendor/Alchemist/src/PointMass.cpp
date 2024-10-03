
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

	/*void Contact2D::resolve(float dt) {
		resolve_velocity(dt);
	}

	float Contact2D::calculate_seperating_velocity() const {
		glm::vec2 relative_velocity = particles.first->get_velocity();
		if (particles.second)
			relative_velocity -= particles.second;
		return glm::dot(relative_velocity, contact_normal);
	}

	void Contact2D::resolve_velocity(float dt) {
		float seperating_velocity = calculate_seperating_velocity();

		if (seperating_velocity > 0)
			return;

		float new_seperating_velocity = -seperating_velocity * restitution;
		float velocity_difference = new_seperating_velocity - seperating_velocity;

		float total_inverse_mass = particles.first->get_mass_inverse();
		if (particles.second)
			total_inverse_mass += particles.second->get_mass_inverse();
		if (total_inverse_mass < 0)
			return;

		float impulse = velocity_difference / total_inverse_mass;
		glm::vec2 impuler_per_inverse_mass = contact_normal * impulse;

		particles.first->set_velocity(particles.first->get_velocity() + impuler_per_inverse_mass * particles.first->get_mass_inverse());
		if(particles.second)
			particles.second->set_velocity(particles.second->get_velocity() + impuler_per_inverse_mass * -particles.second->get_mass_inverse());
	}*/
}