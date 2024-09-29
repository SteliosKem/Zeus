#pragma once
#include "Core/Timestep.h"
#include "Entity.h"

namespace Ivory {
	class ScriptableEntity {
	public:
		template<typename T>
		T& get_component() {
			return m_entity.get_component<T>();
		}
	protected:
		virtual void on_create() {}
		virtual void on_destroy() {}
		virtual void on_update(Timestep dt) {}
	private:
		Entity m_entity;
		friend class Scene;
	};
}