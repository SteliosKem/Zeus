#pragma once
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "PointMass.h"

namespace Ivory {
	class Snapshot {
	public:
		Snapshot() = default;
		Snapshot(const std::shared_ptr<Scene>& scene, bool first = false) : m_scene_context{ scene } {
			auto view = scene->get_registry().view<PointMassComponent>();
			for (auto& e : view) {
				Alchemist::PointMass2D* point_mass = &scene->get_registry().get<PointMassComponent>(e).point_mass;
				if (point_mass->is_static())
					continue;
				point_mass->set_position(scene->get_registry().get<TransformComponent>(e).translation);
				m_point_masses[e] = *point_mass;
			}
		}
		void retrieve(const std::shared_ptr<Scene>& scene = nullptr) {
			for (auto& [e, point_mass] : m_point_masses) {
				(scene ? scene : m_scene_context)->get_registry().get<PointMassComponent>(e).point_mass = point_mass;
				(scene ? scene : m_scene_context)->get_registry().get<TransformComponent>(e).translation = { point_mass.get_position(), 0 };
			}
		}
	private:
		std::map<entt::entity, Alchemist::PointMass2D> m_point_masses;
		
		std::shared_ptr<Scene> m_scene_context{ nullptr };
	};

	class SnapshotManager {
	public:
		SnapshotManager() = default;
		SnapshotManager(const std::shared_ptr<Scene>& scene) : m_scene_context{ scene } {}
		void first_snapshot() {
			m_first = Snapshot(m_scene_context, true);
			m_snapshots.push_back(m_first);
		}
		void record_snapshot(const std::shared_ptr<Scene>& scene = nullptr) {
			m_snapshots.push_back(Snapshot(scene ? scene : m_scene_context));
		}
		void retrieve_snapshot(uint64_t frame, const std::shared_ptr<Scene>& scene = nullptr) {
			m_snapshots[frame].retrieve(scene);
		}
		void reset() {
			m_snapshots.clear();
		}
		bool empty() const {
			return m_snapshots.empty();
		}
		void set_scene(std::shared_ptr<Scene> scene) {
			m_scene_context = scene;
		}
	private:
		Snapshot m_first;
		std::vector<Snapshot> m_snapshots;
		std::shared_ptr<Scene> m_scene_context{ nullptr };
	};
}