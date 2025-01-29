#pragma once
#include <glm/glm.hpp>
#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Core/Uuid.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Rendering/Texture.h"
#include <unordered_map>

#include "PointMass.h"

namespace Ivory {
	enum PhysicsObjectType {
		PointMassCircle,
		PointMassSquare,
		Wall,
		Floor,
		Spring,
		Rope,
		Rod
	};

	struct IdComponent {
		Uuid id;
		IdComponent() = default;
		IdComponent(const IdComponent&) = default;
	};

	struct ObjectTypeComponent {
		PhysicsObjectType type;
		ObjectTypeComponent() = default;
		ObjectTypeComponent(const ObjectTypeComponent&) = default;
	};

	struct TagComponent {
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& str) : tag(str) {}
	};

	struct TransformComponent {
		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : translation(translation) {}

		glm::mat4 get_transform() const {
			glm::mat4 _rotation = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4{ 1.0f }, translation) * _rotation * glm::scale(glm::mat4{ 1.0f }, scale);
		}

		// Returns a vector of the points of a quad, transformed accordingly with the object's
		// current transformation matrix
		std::vector<glm::vec2> get_transformed_points() const {
			std::array<glm::vec4, 4> points;
			std::vector<glm::vec2> to_ret(4);
			points[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
			points[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
			points[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
			points[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
			glm::mat4 transform = get_transform();
			for (int i = 0; i < 4; i++) {
				to_ret[i] = transform * points[i];
			}
			return to_ret;
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color{ 1.0f };
		std::shared_ptr<Texture2D> texture{ nullptr };
		float tiling_factor = 1.0f;
		
		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : color(color) {}
	};

	struct CircleRendererComponent {
		glm::vec4 color{ 1.0f };
		float radius = 0.5f;
		float thickness = 1.0f;
		float fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent {
		SceneCamera camera;
		bool active = true;
		bool fixed_aspect_ratio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct CScriptComponent {
		ScriptableEntity* instance = nullptr;

		ScriptableEntity* (*instantiate_script)();
		void (*destroy_script)(CScriptComponent*);

		template<typename T>
		void bind() {
			instantiate_script = []() {return static_cast<ScriptableEntity*>(new T()); };
			destroy_script = [](CScriptComponent* script_component) {delete script_component->instance; script_component->instance = nullptr; };
		}
	};

	struct PointMassComponent {
		struct ForceInfo {
			glm::vec2 force_vector;
			bool use_angle = false;
			float force_length;
			float angle;
		};
		bool show_velocity;
		bool is_circle = false;
		Alchemist::PointMass2D point_mass = {1.0f};
		bool affected_by_gravity = true;
		std::unordered_map<std::string, ForceInfo> forces_info;
		int force_counter = 0;
		std::vector<Alchemist::SimpleForce> forces;
		bool ignore_collisions = false;
		bool will_update = true;

		PointMassComponent() = default;
		PointMassComponent(const PointMassComponent&) = default;
	};

	struct SpringComponent {
		Alchemist::PointMassSpring spring = {nullptr, 1.0f, 1.0f};
		Alchemist::PointMass2D* first_object = nullptr;
		Alchemist::PointMass2D* second_object = nullptr;
		Uuid first_object_id = 0;
		Uuid second_object_id = 0;
		float height = 2.0f;
		int revolutions = 20;

		SpringComponent() = default;
		SpringComponent(const SpringComponent&) = default;
	};

	struct AnchoredSpringComponent {
		Alchemist::PointMassSpring spring = { nullptr, 1.0f, 1.0f };
		Alchemist::PointMass2D default_object;
		Alchemist::PointMass2D anchor_object;
		Alchemist::PointMass2D* attached_object;
		Uuid attached_object_id = 0;

		AnchoredSpringComponent() = default;
		AnchoredSpringComponent(const AnchoredSpringComponent&) = default;
	};

	struct CableComponent {
		Alchemist::Cable cable{ 1.0f };
		Alchemist::PointMass2D* first_object;
		Alchemist::PointMass2D* second_object;
		Uuid first_object_id = 0;
		Uuid second_object_id = 0;

		CableComponent() = default;
		CableComponent(const CableComponent&) = default;
	};

	struct AnchoredCableComponent {
		Alchemist::Cable cable{ 1.0f };
		Alchemist::PointMass2D default_object;
		Alchemist::PointMass2D anchor_object;
		Alchemist::PointMass2D* attached_object;
		Uuid attached_object_id = 0;

		AnchoredCableComponent() = default;
		AnchoredCableComponent(const AnchoredCableComponent&) = default;
	};

	struct RodComponent {
		Alchemist::Rod rod{ 1.0f };
		Alchemist::PointMass2D* first_object;
		Alchemist::PointMass2D* second_object;
		Uuid first_object_id = 0;
		Uuid second_object_id = 0;

		RodComponent() = default;
		RodComponent(const RodComponent&) = default;
	};

	struct AnchoredRodComponent {
		Alchemist::Rod rod{ 1.0f };
		Alchemist::PointMass2D default_object;
		Alchemist::PointMass2D anchor_object;
		Alchemist::PointMass2D* attached_object;
		Uuid attached_object_id = 0;

		AnchoredRodComponent() = default;
		AnchoredRodComponent(const AnchoredRodComponent&) = default;
	};

	struct GravityComponent {
		Alchemist::PointMassGravity gravity;

		GravityComponent() = default;
		GravityComponent(const GravityComponent&) = default;
	};
}