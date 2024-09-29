#include "pch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Components.h"
#include "Project/Project.h"

namespace YAML {
	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& right) {
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.push_back(right.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& right) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			right.x = node[0].as<float>();
			right.y = node[1].as<float>();
			right.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& right) {
			Node node;
			node.push_back(right.x);
			node.push_back(right.y);
			node.push_back(right.z);
			node.push_back(right.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& right) {
			if (!node.IsSequence() || node.size() != 4)
				return false;
			right.x = node[0].as<float>();
			right.y = node[1].as<float>();
			right.z = node[2].as<float>();
			right.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Ivory {
	SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : m_scene{ scene } {

	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void serialize_entity(YAML::Emitter& out, Entity entity) {
		IV_CORE_ASSERT(entity.has_component<IdComponent>());

		out << YAML::BeginMap;
		out << YAML::Key << "Entity";
		out << YAML::Value << entity.get_component<IdComponent>().id;

		if (entity.has_component<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag";
			out << YAML::Value << entity.get_component<TagComponent>().tag;
			out << YAML::EndMap;
		}
		if (entity.has_component<TransformComponent>()) {
			auto& transform = entity.get_component<TransformComponent>();
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Translation";
			out << YAML::Value << transform.translation;
			out << YAML::Key << "Rotation";
			out << YAML::Value << transform.rotation;
			out << YAML::Key << "Scale";
			out << YAML::Value << transform.scale;
			out << YAML::EndMap;
		}
		if (entity.has_component<CameraComponent>()) {
			auto& camera = entity.get_component<CameraComponent>();
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Camera";
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.camera.get_projection_type();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.camera.get_vertical_fov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.camera.get_perspective_near_clip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.camera.get_perspective_far_clip();

			out << YAML::Key << "OrthoSize" << YAML::Value << camera.camera.get_ortho_size();
			out << YAML::Key << "OrthoNear" << YAML::Value << camera.camera.get_ortho_near_clip();
			out << YAML::Key << "OrthoFar" << YAML::Value << camera.camera.get_ortho_far_clip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << camera.active;
			out << YAML::Key << "FixedAspectRation" << YAML::Value << camera.fixed_aspect_ratio;

			out << YAML::EndMap;
		}
		if (entity.has_component<SpriteRendererComponent>()) {
			auto& sprite_renderer_component = entity.get_component<SpriteRendererComponent>();
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color";
			out << YAML::Value << entity.get_component<SpriteRendererComponent>().color;
			if (sprite_renderer_component.texture) {
				out << YAML::Key << "TexturePath" << YAML::Value << sprite_renderer_component.texture->get_path().string();
			}
			out << YAML::Key << "TilingFactor" << YAML::Value << sprite_renderer_component.tiling_factor;
			out << YAML::EndMap;
		}
		if (entity.has_component<CircleRendererComponent>()) {
			CircleRendererComponent comp = entity.get_component<CircleRendererComponent>();
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color";
			out << YAML::Value << comp.color;
			out << YAML::Key << "Thickness";
			out << YAML::Value << comp.thickness;
			out << YAML::Key << "Fade";
			out << YAML::Value << comp.fade;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialize(const std::string& filepath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Name";
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		auto view = m_scene->m_registry.view<Entity::all_entities>();
		for (auto entityid : view) {
			Entity entity{ entityid, m_scene.get() };
			if (!entity)
				break;

			serialize_entity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}
	void SceneSerializer::serialize_runtime(const std::string& filepath) {
		IV_CORE_ASSERT(false, "serialize_runtime is not supported");
	}

	bool SceneSerializer::deserialize(const std::string& filepath) {
		std::ifstream stream(filepath);
		std::stringstream str_stream;
		str_stream << stream.rdbuf();

		YAML::Node data = YAML::Load(str_stream.str());
		if (!data["Scene"])
			return false;
		std::string scene_name = data["Scene"].as<std::string>();
		IV_CORE_TRACE("Deserializing scene '{0}'", scene_name);

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tag_component = entity["TagComponent"];
				if (tag_component)
					name = tag_component["Tag"].as<std::string>();

				IV_CORE_TRACE("Deserialized entity with ID = {0} and name = {1}", uuid, name);
			
				Entity deserialized_entity = m_scene->create_entity_with_uuid(uuid, name);
				IV_INFO((uint64_t)deserialized_entity.get_component<IdComponent>().id);

				auto transform_component = entity["TransformComponent"];
				if (transform_component) {
					auto& component = deserialized_entity.get_component<TransformComponent>();
					component.translation = transform_component["Translation"].as<glm::vec3>();
					component.rotation = transform_component["Rotation"].as<glm::vec3>();
					component.scale = transform_component["Scale"].as<glm::vec3>();
				}

				auto camera_component = entity["CameraComponent"];
				if (camera_component) {
					auto& camera_props = camera_component["Camera"];
					auto& component = deserialized_entity.add_component<CameraComponent>();
					component.active = camera_component["Primary"].as<bool>();
					component.fixed_aspect_ratio = camera_component["FixedAspectRation"].as<bool>();

					component.camera.set_ortho_far_clip(camera_props["OrthoFar"].as<float>());
					component.camera.set_ortho_near_clip(camera_props["OrthoNear"].as<float>());
					component.camera.set_ortho_size(camera_props["OrthoSize"].as<float>());

					component.camera.set_perspective_far_clip(camera_props["PerspectiveFar"].as<float>());
					component.camera.set_perspective_near_clip(camera_props["PerspectiveNear"].as<float>());
					component.camera.set_vertical_fov(camera_props["PerspectiveFOV"].as<float>());

					component.camera.set_projection_type((SceneCamera::ProjectionType)camera_props["ProjectionType"].as<int>());
				}

				auto sprite_renderer_component = entity["SpriteRendererComponent"];
				if (sprite_renderer_component) {
					auto& component = deserialized_entity.add_component<SpriteRendererComponent>();
					component.color = sprite_renderer_component["Color"].as<glm::vec4>();
					if(sprite_renderer_component["TexturePath"])
						component.texture = Texture2D::create((Project::get_assets_dir() / sprite_renderer_component["TexturePath"].as<std::string>()).string());
					component.tiling_factor = sprite_renderer_component["TilingFactor"].as<float>();
				}

				auto circle_renderer_component = entity["CircleRendererComponent"];
				if (circle_renderer_component) {
					auto& component = deserialized_entity.add_component<CircleRendererComponent>();
					component.color = circle_renderer_component["Color"].as<glm::vec4>();
					component.fade = circle_renderer_component["Fade"].as<float>();
					component.thickness = circle_renderer_component["Thickness"].as<float>();
				}
			}
		}
		return true;
	}
	bool SceneSerializer::deserialize_runtime(const std::string& filepath) {
		IV_CORE_ASSERT(false, "deserialize_runtime is not supported");
		return false;
	}
}