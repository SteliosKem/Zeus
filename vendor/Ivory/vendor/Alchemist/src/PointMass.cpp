#include "PointMass.h"
#include <limits>
#include <iostream>

namespace Alchemist {
	void PointMass2D::integrate(float dt) {
		glm::vec2 final_acceleration = m_acceleration + m_force_accumulator * get_mass_inverse();
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

	// Unused
	void resolve_elastic_collision_circle(PointMass2D& first, PointMass2D& second) {
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

	Collision check_circle_collision_depth(const PointMass2D& first, const PointMass2D& second) {
		// Check if circles intercept, so check if the distance of their centers is
		// less than the sum of their radi
		float distance_of_centers_squared = (first.get_position().x - second.get_position().x) * (first.get_position().x - second.get_position().x)
			+ (first.get_position().y - second.get_position().y) * (first.get_position().y - second.get_position().y);

		float sum_of_radi_squared = (first.get_shape().radius + second.get_shape().radius) * (first.get_shape().radius + second.get_shape().radius);
		if (distance_of_centers_squared < sum_of_radi_squared)
			return Collision{first.get_shape().radius + second.get_shape().radius - sqrtf(distance_of_centers_squared), glm::normalize(first.get_position() - second.get_position())};
		return Collision{};
	}

	static bool loop_through_vertices(const std::vector<glm::vec2>& first_polygons, const std::vector<glm::vec2>& second_polygons
		, float& in_out_depth, glm::vec2& in_out_normal) {
		// Find if there is a seperating axis
		// If not, output the depth and normal of the collision
		for (int i = 0; i < first_polygons.size(); i++) {
			glm::vec2 vertex_a = first_polygons[i];
			glm::vec2 vertex_b = first_polygons[(i + 1) % first_polygons.size()];	// <-- loop back if i exceeds the size of first_polygons

			glm::vec2 edge = vertex_b - vertex_a;
			glm::vec2 seperating_axis = glm::vec2(-edge.y, edge.x);
			seperating_axis = glm::normalize(seperating_axis);

			glm::vec2 min_max_a = project_vertices(first_polygons, seperating_axis);
			glm::vec2 min_max_b = project_vertices(second_polygons, seperating_axis);

			if (min_max_a.x >= min_max_b.y || min_max_b.x >= min_max_a.y) return true;

			float axis_depth = fmin(min_max_b.y - min_max_a.x, min_max_a.y - min_max_b.x);

			if (axis_depth < in_out_depth) {
				in_out_depth = axis_depth;
				in_out_normal = seperating_axis;
			}
		}
		return false;
	}

	glm::vec2 find_mean_vector(const std::vector<glm::vec2>& vectors) {
		glm::vec2 to_ret;
		for (auto& vec : vectors) {
			to_ret += vec;
		}
		to_ret /= vectors.size();
		return to_ret;
	}

	Collision check_sat_collision(const std::vector<glm::vec2>& first_polygons, const std::vector<glm::vec2>& second_polygons) {
		float depth = std::numeric_limits<float>::max();
		glm::vec2 normal{ 0.0f };
		
		// If seperating axis are found return no collision
		if (loop_through_vertices(first_polygons, second_polygons, depth, normal)) return Collision{};
		if (loop_through_vertices(second_polygons, first_polygons, depth, normal)) return Collision{};

		glm::vec2 center_a = find_mean_vector(first_polygons);
		glm::vec2 center_b = find_mean_vector(second_polygons);

		if (glm::dot(center_b - center_a, normal) > 0)
			normal = -normal;

		return Collision{ depth, normal };
	}

	glm::vec2 project_circle(const glm::vec2& center, float radius, const glm::vec2& axis) {
		glm::vec2 direction = glm::normalize(axis);
		direction *= radius;
		glm::vec2 point_a = center + direction;
		glm::vec2 point_b = center - direction;

		glm::vec2 to_ret = {glm::dot(point_a, axis), glm::dot(point_b, axis)};
		if (to_ret.y < to_ret.x) {
			float a = to_ret.x;
			to_ret.x = to_ret.y;
			to_ret.y = a;
		}
		return to_ret;
	}

	static int closest_point(const glm::vec2& point, const std::vector<glm::vec2>& polygon) {
		int result = -1;
		float min_distance = std::numeric_limits<float>::max();

		for (int i = 0; i < polygon.size(); i++) {
			glm::vec2 vertex = polygon[i];
			float distance = glm::distance(vertex, point);

			if (distance < min_distance) {
				min_distance = distance;
				result = i;
			}
		}
		return result;
	}

	Collision check_sat_circle_collision(const glm::vec2& circle_center, float circle_radius, const std::vector<glm::vec2>& polygons) {
		float depth = std::numeric_limits<float>::max();
		glm::vec2 normal{ 0.0f };
		for (int i = 0; i < polygons.size(); i++) {
			glm::vec2 vertex_a = polygons[i];
			glm::vec2 vertex_b = polygons[(i + 1) % polygons.size()];

			glm::vec2 edge = vertex_b - vertex_a;
			glm::vec2 seperating_axis = glm::vec2(-edge.y, edge.x);
			seperating_axis = glm::normalize(seperating_axis);

			glm::vec2 min_max_a = project_vertices(polygons, seperating_axis);
			glm::vec2 min_max_b = project_circle(circle_center, circle_radius, seperating_axis);

			if (min_max_a.x >= min_max_b.y || min_max_b.x >= min_max_a.y) return Collision{};

			float axis_depth = fmin(min_max_b.y - min_max_a.x, min_max_a.y - min_max_b.x);

			if (axis_depth < depth) {
				depth = axis_depth;
				normal = seperating_axis;
			}
		}

		int closest_point_index = closest_point(circle_center, polygons);
		glm::vec2 closest_point = polygons[closest_point_index];
		glm::vec2 seperating_axis = closest_point - circle_center;

		glm::vec2 min_max_a = project_vertices(polygons, seperating_axis);
		glm::vec2 min_max_b = project_circle(circle_center, circle_radius, seperating_axis);

		if (min_max_a.x >= min_max_b.y || min_max_b.x >= min_max_a.y) return Collision{};

		float axis_depth = fmin(min_max_b.y - min_max_a.x, min_max_a.y - min_max_b.x);

		if (axis_depth < depth) {
			depth = axis_depth;
			normal = seperating_axis;
		}

		glm::vec2 center_b = find_mean_vector(polygons);

		if (glm::dot(center_b - circle_center, normal) < 0)
			normal = -normal;

		return Collision{ depth, normal };
	}

	// Projects each point provided to a line for use in the SAT
	glm::vec2 project_vertices(const std::vector<glm::vec2>& vertices, const glm::vec2& axis) {
		float min = std::numeric_limits<float>::max();
		float max = std::numeric_limits<float>::min();

		for (int i = 0; i < vertices.size(); i++) {
			glm::vec2 vertex = vertices[i];
			float projection = glm::dot(vertex, axis);

			if (projection < min) min = projection;
			if (projection > max) max = projection;
		}

		return {min, max};
	}

	// Adjusts position of each colliding body accordingly
	void resolve_plain_collision(PointMass2D* first, PointMass2D* second, const Collision& collision) {
		// If both objects are static then don't do anything
		if (first->is_static() && second->is_static())
			return;

		// If one object is static move the other by the full collision depth
		if (first->is_static()) {
			second->set_position(second->get_position() - collision.collision_normal * collision.depth);
		}
		else if(second->is_static())
			first->set_position(first->get_position() + collision.collision_normal * collision.depth);

		// Else move both objects half of the collision depth
		else {
			first->set_position(first->get_position() + collision.collision_normal * collision.depth / 2.0f);
			second->set_position(second->get_position() - collision.collision_normal * collision.depth / 2.0f);
		}
	}

	void resolve_collision(PointMass2D* first, PointMass2D* second, const Collision& collision) {
		glm::vec2 relative_velocity = second->get_velocity() - first->get_velocity();
		resolve_plain_collision(first, second, collision);
		if (first->does_only_collide_plainly() || second->does_only_collide_plainly())
			return;
		if (glm::dot(relative_velocity, collision.collision_normal) < 0)
			return;

		float impulse = ( - (1.0f + collision.restitution) * glm::dot(relative_velocity, collision.collision_normal) )
						/ ( first->get_mass_inverse() + second->get_mass_inverse() );
		first->set_velocity(first->get_velocity() - impulse * first->get_mass_inverse() * collision.collision_normal);
		second->set_velocity(second->get_velocity() + impulse * second->get_mass_inverse() * collision.collision_normal);
	}

	void resolve_collision_with_friction(PointMass2D* first, PointMass2D* second, const Collision& collision) {
		glm::vec2 relative_velocity = second->get_velocity() - first->get_velocity();
		resolve_plain_collision(first, second, collision);
		if (first->does_only_collide_plainly() || second->does_only_collide_plainly())
			return;
		if (glm::dot(relative_velocity, collision.collision_normal) < 0)
			return;

		float impulse = (-(1.0f + collision.restitution) * glm::dot(relative_velocity, collision.collision_normal))
			/ (first->get_mass_inverse() + second->get_mass_inverse());
		first->set_velocity(first->get_velocity() - impulse * first->get_mass_inverse() * collision.collision_normal);
		second->set_velocity(second->get_velocity() + impulse * second->get_mass_inverse() * collision.collision_normal);
		
		glm::vec2 normal_tangent = relative_velocity - glm::dot(relative_velocity, collision.collision_normal) * collision.collision_normal;
		float friction_impulse = -glm::dot(relative_velocity, normal_tangent)
			/ (first->get_mass_inverse() + second->get_mass_inverse());
		float dynamic_friction = fmaxf(first->get_dynamic_friction_factor(), second->get_dynamic_friction_factor());
		float static_friction = fmaxf(first->get_static_friction_factor(), second->get_static_friction_factor());

		glm::vec2 friction_vector;

		if (fabs(friction_impulse) <= impulse * static_friction)
			friction_vector = friction_impulse * normal_tangent;
		else
			friction_vector = impulse * normal_tangent * dynamic_friction;

		first->set_velocity(first->get_velocity() - friction_vector * first->get_mass_inverse());
		second->set_velocity(second->get_velocity() + friction_vector * second->get_mass_inverse());
	}

	float Cable::get_current_length() const {
		glm::vec2 relative_pos = first->get_position() - second->get_position();
		return glm::length(relative_pos);
	}

	uint32_t Cable::fill_collision(Collision* collision, uint32_t limit) const {
		float length = get_current_length();

		if (length <= m_max_length)
			return 0;

		collision->first = first;
		collision->second = second;
		glm::vec2 normal = second->get_position() - first->get_position();
		normal = glm::normalize(normal);
		collision->collision_normal = normal;
		collision->depth = length - m_max_length;

		return 1;
	}

	float Rod::get_current_length() const {
		glm::vec2 relative_pos = first->get_position() - second->get_position();
		return glm::length(relative_pos);
	}

	uint32_t Rod::fill_collision(Collision* collision, uint32_t limit) const {
		float length = get_current_length();

		if (length == m_length)
			return 0;

		collision->first = first;
		collision->second = second;
		glm::vec2 normal = second->get_position() - first->get_position();
		normal = glm::normalize(normal);
		if (length > m_length) {
			collision->collision_normal = normal;
			collision->depth = length - m_length;
		}
		else {
			collision->collision_normal = -normal;
			collision->depth = m_length - length;
		}

		collision->restitution = 0;

		return 1;
	}

	std::vector<ForceGenerator2D*> ForceRegistry::get_forces(PointMass2D* point_mass) {
		std::vector<ForceGenerator2D*> forces;
		for (auto& i : registrations) {
			if (i.point_mass == point_mass)
				forces.push_back(i.force_generator);
		}
		return forces;
	}
} 