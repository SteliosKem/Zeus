#include "pch.h"
#include "LayerStack.h"

namespace Ivory {
	LayerStack::LayerStack() {}

	LayerStack::~LayerStack() {}

	void LayerStack::push_layer(shared_ptr<Layer> layer) {
		m_layers.emplace(m_layers.begin() + m_layer_insert_index, layer);
		m_layer_insert_index++;
	}

	void LayerStack::push_overlay(shared_ptr<Layer> overlay) {
		m_layers.emplace_back(overlay);
	}

	void LayerStack::pop_layer(shared_ptr<Layer> layer) {
		auto it = std::find(m_layers.begin(), m_layers.end(), layer);
		if (it != m_layers.end()) {
			m_layers.erase(it);
			m_layer_insert_index--;
		}
	}

	void LayerStack::pop_overlay(shared_ptr<Layer> overlay) {
		auto it = std::find(m_layers.begin(), m_layers.end(), overlay);
		if (it != m_layers.end())
			m_layers.erase(it);
	}
}