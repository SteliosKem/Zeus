#pragma once
#include "glm/glm.hpp"
#include <utility>

namespace Alchemist {
	struct Circle {
		float radius = .5f;
		glm::vec2 center{ 0.0f };
	};

	// Collision Detection
	bool check_circles(const Circle& first, const Circle& second);

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
		void set_restitution(float res) { m_restitution = res; }

		// Getters
		glm::vec2 get_position() const { return m_position; }
		glm::vec2 get_velocity() const { return m_velocity; }
		glm::vec2 get_acceleration() const { return m_acceleration; }

		glm::vec2& get_position() { return m_position; }
		glm::vec2& get_velocity() { return m_velocity; }
		glm::vec2& get_acceleration() { return m_acceleration; }
		float get_mass() const { return 1 / m_mass_inverse; }
		float get_mass_inverse() const { return (m_is_static) ? 0 : m_mass_inverse; }
		float get_damping() const { return m_damping; }
		float& get_damping() { return m_damping; }
		float& get_restitution() { return m_restitution; }

		bool is_static() const { return m_is_static; }
		void set_static(bool set) { m_is_static = set; }

		bool does_only_collide_plainly() const { return m_plain_collide; }
		void set_only_collide_plainly(bool set) { m_plain_collide = set; }

		// Call every frame
		void on_update(float dt) { integrate(dt); }
		void add_force(const glm::vec2& force) { m_force_accumulator += force; }
		Circle& get_shape() { return m_shape; }
		Circle get_shape() const { return m_shape; }
	private:
		void integrate(float dt);
		void clear_force_accumulator() { m_force_accumulator = { 0.0f, 0.0f }; }
		
	private:
		Circle m_shape;

		glm::vec2 m_position;
		glm::vec2 m_velocity;
		glm::vec2 m_acceleration;

		// The closest damping is to 1, the lower the effect of it
		float m_damping = 1;

		// Holds the inverse of the point's mass, since it is more useful for calculations
		float m_mass_inverse;
		float m_restitution = 1.0f;

		bool m_is_static = false;
		bool m_plain_collide = false;

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

	class SimpleForce : public ForceGenerator2D {
	public:
		SimpleForce(const glm::vec2& force) : m_force{ force } {}
		void update_force(PointMass2D* point_mass, float dt) override;

		void set_force(const glm::vec2& force) { m_force = force; }
		glm::vec2& get_force() { return m_force; }

	private:
		glm::vec2 m_force;
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

		void first_object_affected(bool set) { m_first_object_affected = set; }
		bool& get_first_object_affected() { return m_first_object_affected; }
	private:
		PointMass2D* m_attached_object;
		glm::vec2 m_spring_length;
		float m_spring_constant;
		float m_spring_rest_length;

		bool m_first_object_affected = true;
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

	struct Collision {
		float depth = 0;
		glm::vec2 collision_normal{ 0.0f };
		float restitution = 1.0f;

		PointMass2D* first{ nullptr };
		PointMass2D* second{ nullptr };
	};

	// Collision Detection

	bool check_collision(const PointMass2D& first, const PointMass2D& second);
	Collision check_circle_collision_depth(const PointMass2D& first, const PointMass2D& second);
	Collision check_sat_collision(const std::vector<glm::vec2>& first_polygons, const std::vector<glm::vec2>& second_polygons);
	Collision check_sat_circle_collision(const glm::vec2& circle_center, float circle_radius, const std::vector<glm::vec2>& polygons);
	glm::vec2 project_vertices(const std::vector<glm::vec2>& vertices, const glm::vec2& axis);
	glm::vec2 project_circle(const glm::vec2& center, float radius, const glm::vec2& axis);

	void resolve_elastic_collision_circle(PointMass2D& first, PointMass2D& second);
	void resolve_plain_collision(PointMass2D* first, PointMass2D* second, const Collision& collision);
	void resolve_collision(PointMass2D* first, PointMass2D* second, const Collision& collision);

	// Constraints

	class Cable {
	public:
		Cable(float max_length = 0.0f) { m_max_length = max_length; }
		uint32_t fill_collision(Collision* collision, uint32_t limit) const;
		float get_current_length() const;

		void set_max_length(float length) { m_max_length = length; }
		float& get_max_length() { return m_max_length; }

		PointMass2D* first{ nullptr }, * second{ nullptr };
	private:
		float m_max_length;
		
	};
}