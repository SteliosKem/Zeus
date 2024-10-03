#pragma once
#include <glm/glm.hpp>
#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include "Core/Uuid.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "Rendering/Texture.h"

#include "PointMass.h"

namespace Ivory {
	struct IdComponent {
		Uuid id;
		IdComponent() = default;
		IdComponent(const IdComponent&) = default;
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
		Alchemist::PointMass2D point_mass = {1.0f};
		bool affected_by_gravity = true;

		PointMassComponent() = default;
		PointMassComponent(const PointMassComponent&) = default;
	};

	struct SpringComponent {
		Alchemist::PointMassSpring spring = {nullptr, 1.0f, 1.0f};
		Alchemist::PointMass2D* first_object;
		Alchemist::PointMass2D* second_object;
		Uuid first_object_id;
		Uuid second_object_id;

		SpringComponent() = default;
		SpringComponent(const SpringComponent&) = default;
	};

	struct GravityComponent {
		Alchemist::PointMassGravity gravity;

		GravityComponent() = default;
		GravityComponent(const GravityComponent&) = default;
	};
}