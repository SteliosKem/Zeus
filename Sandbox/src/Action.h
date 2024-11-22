#pragma once
#include "Scene/Entity.h"
#include "Snapshot.h"
#include <deque>

namespace Ivory {
	enum ActionType {
		Create,
		Delete,
		Move,
		Rotate,
		Scale,
		ChangeFloatField,
		ChangeVectorField,
		DeleteRecording
	};

	class Action {
	public:
		Action(ActionType action_type, Entity entity)
			: action{ action_type }, entity{ entity } {}
		Action(ActionType action_type, Entity entity, Entity before_entity)
			: action{ action_type }, entity{ entity }, before_entity{ before_entity } {}
		Action(ActionType action_type, Entity entity, float* field, float before, float after)
			: action{ action_type }, entity{ entity }, float_field{ field }, float_before{ before }, float_after{ after } {}
		Action(ActionType action_type, Entity entity, glm::vec2* field, const glm::vec2& before, const glm::vec2& after)
			: action{ action_type }, entity{ entity }, vector_field{ field }, vector_before{ before }, vector_after{ after } {}
		Action(ActionType action_type, const SnapshotManager& before_manager, const SnapshotManager& after_manager)
			: action{ action_type }, before_manager{ before_manager }, after_manager{ after_manager } {}

		ActionType action;
		Entity entity;
		Entity before_entity;
		float* float_field;
		float float_before;
		float float_after;
		glm::vec2* vector_field;
		glm::vec2 vector_before;
		glm::vec2 vector_after;
		SnapshotManager before_manager;
		SnapshotManager after_manager;
	};

	class ActionManager {
	public:
		ActionManager() = default;
		ActionManager(SnapshotManager* snapshot_manager) : m_manager{ snapshot_manager } {}
		ActionManager(uint32_t max_history) : m_max_history{ max_history } {}

		void undo();
		void redo() {}
		void action(const std::shared_ptr<Scene>& scene, const Action& action);
		void set_snapshot_manager(SnapshotManager* manager) {
			m_manager = manager;
		}
	private:
		std::deque<std::pair<std::shared_ptr<Scene>, Action>> m_actions;
		uint32_t m_max_history = 20;
		SnapshotManager* m_manager = nullptr;
	};
}