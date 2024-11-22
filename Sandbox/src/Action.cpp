#include "Action.h"

namespace Ivory {
	void ActionManager::action(const std::shared_ptr<Scene>& scene, const Action& action) {
		if (m_actions.size() == m_max_history)
			m_actions.pop_front();
		m_actions.push_back({scene, action});
	}

	void ActionManager::undo() {
		if (m_actions.empty())
			return;
		auto [scene, _action] = m_actions.back();
		m_actions.pop_back();
		switch (_action.action) {
			case Create: {
				entt::entity entity = _action.entity;
				scene->set_selected_entity({});
				scene->destroy_entity(_action.entity);
				break;
			}
			case Delete: {
				//entt::entity entity = _action.entity;
				//scene->copy_entity(_action.entity);
				break;
			}
			case Move: {
				_action.entity.get_component<TransformComponent>().translation = { _action.vector_before, 0 };
				break;
			}
			case Rotate: {
				_action.entity.get_component<TransformComponent>().rotation = { _action.vector_before, 0 };
				break;
			}
			case Scale: {
				_action.entity.get_component<TransformComponent>().scale = { _action.vector_before, 0 };
				break;
			}
			case ChangeFloatField: {
				*_action.float_field = _action.float_before;
				break;
			}
			case ChangeVectorField: {
				*_action.vector_field = _action.vector_before;
				break;
			}
			case DeleteRecording: {
				*m_manager = _action.before_manager;
				break;
			}
		}
	}
}