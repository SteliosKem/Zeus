#pragma once

#include "Core/Core.h"
#include "Layer.h"

#include <vector>

using std::shared_ptr;

namespace Ivory {
	class IVORY_API LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void push_layer(shared_ptr<Layer> layer);
		void push_overlay(shared_ptr<Layer> overlay);
		void pop_layer(shared_ptr<Layer> layer);
		void pop_overlay(shared_ptr<Layer> overlay);

		std::vector<shared_ptr<Layer>>::iterator begin() { return m_layers.begin(); }
		std::vector<shared_ptr<Layer>>::iterator end() { return m_layers.end(); }
	private:
		std::vector<shared_ptr<Layer>> m_layers;
		int m_layer_insert_index = 0;
	};
}