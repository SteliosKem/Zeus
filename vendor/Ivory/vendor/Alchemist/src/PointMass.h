#pragma once
#include "glm/glm.hpp"
#include <utility>

namespace Alchemist {
	class PointMass2D {
	public:
		PointMass2D(float mass_inverse, float damping = 1, const glm::vec2& acceleration = {0.0f, 0.0f}, 
			const glm::vec2& velocity = { 0.0f, 0.0f }, const glm::vec2& position = { 0.0f, 0.0f })
			: m_acceleration{ acceleration }, m_velocity{ velocity }, m_position{ position }
			, m_mass_inverse{ mass_inverse }, m_damping{damping} {}

		// Setters
		void set_acceleration(const glm::vec2& a) { m_acceleration = a; }
		void set_velocity(const glm::vec2& a) { m_velocity = a; }
		void set_position(const glm::vec2& a) { m_position = a; }
		void set_mass(float mass) { m_mass_inverse = 1 / mass; }
		void set_damp(float damp) { m_damping = damp; }

		// Getters
		glm::vec2 get_position() const { return m_position; }
		glm::vec2 get_velocity() const { return m_velocity; }
		glm::vec2 get_acceleration() const { return m_acceleration; }

		glm::vec2& get_position() { return m_position; }
		glm::vec2& get_velocity() { return m_velocity; }
		glm::vec2& get_acceleration() { return m_acceleration; }
		float get_mass() const { return 1 / m_mass_inverse; }
		float get_mass_inverse() const { return m_mass_inverse; }
		float get_damping() const { return m_damping; }
		float& get_damping() { return m_damping; }

		// Call every frame
		void on_update(float dt) { integrate(dt); }
		void add_force(const glm::vec2& force) { m_force_accumulator += force; }
	private:
		void integrate(float dt);
		void clear_force_accumulator() { m_force_accumulator = { 0.0f, 0.0f }; }
		
	private:
		glm::vec2 m_position;
		glm::vec2 m_velocity;
		glm::vec2 m_acceleration;

		// The closest damping is to 1, the lower the effect of it
		float m_damping = 1;

		// Holds the inverse of the point's mass, since it is more useful for calculations
		float m_mass_inverse;

		// Holds overall force for every update pass
		glm::vec2 m_force_accumulator{0.0f};
		
	};

	class ForceGenerator2D {
	public:
		virtual void update_force(PointMass2D* point_mass, float dt) = 0;
	};

	class ForceRegistry {
	protected:
		struct ForceRegistration {
			PointMass2D* point_mass;
			ForceGenerator2D* force_generator;
		};

		using Registry = std::vector<ForceRegistration>;
		Registry registrations;
	public:
		void add(PointMass2D* point_mass, ForceGenerator2D* force_gen) { registrations.push_back({ point_mass, force_gen }); }
		//void remove(PointMass2D*, ForceGenerator2D* force_gen) { registrations.re }

		void clear() { registrations.clear(); }
		void update_forces(float dt);
	};

	class PointMassGravity : public ForceGenerator2D {
	public:
		PointMassGravity(const glm::vec2& gravity = {0.0f, -9.8f}) : m_gravity{gravity} {}
		void update_force(PointMass2D* point_mass, float dt) override;

		void set_gravity(const glm::vec2& gravity) { m_gravity = gravity; }
		glm::vec2& get_gravity() { return m_gravity; }
	private:
		glm::vec2 m_gravity;
	};

	class PointMassSpring : public ForceGenerator2D {
	public:
		PointMassSpring(PointMass2D* attached, float constant, float rest)
			: m_attached_object{ attached }, m_spring_constant{ constant }, m_spring_rest_length{rest} {}
		void update_force(PointMass2D* point_mass, float dt) override;

		float get_current_spring_length() const { return m_spring_length.length(); }
		float& get_constant() { return m_spring_constant; }
		float& get_rest_length() { return m_spring_rest_length; }

		void set_rest_length(float length) { m_spring_rest_length = length; }
		void set_constant(float constant) { m_spring_constant = constant; }
		void set_attached_object(PointMass2D* obj) { m_attached_object = obj; }
	private:
		PointMass2D* m_attached_object;
		glm::vec2 m_spring_length;
		float m_spring_constant;
		float m_spring_rest_length;
	};

	class AnchoredPointMassSpring : public ForceGenerator2D {
	public:
		AnchoredPointMassSpring(const glm::vec2& position, float constant, float rest)
			: m_position{ position }, m_spring_constant{ constant }, m_spring_rest_length{ rest } {}
		void update_force(PointMass2D* point_mass, float dt) override;
		float get_current_spring_length() const { return m_spring_length.length(); }
	private:
		glm::vec2 m_position;
		glm::vec2 m_spring_length;
		float m_spring_constant;
		float m_spring_rest_length;
	};

	/*class Contact2D {
	public:
		std::pair<PointMass2D*, PointMass2D*> particles;
		float restitution;
		glm::vec2 contact_normal;
		float penetration;
	protected:
		void resolve(float dt);
		float calculate_seperating_velocity() const;
	private:
		void resolve_velocity(float dt);
	};*/
}