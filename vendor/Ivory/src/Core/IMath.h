#pragma once
#include "Core.h"

namespace Ivory {
	class Vector2 {
	public:
		Vector2(Real x = 0, Real y = 0) : m_x(x), m_y(y) {}
		Real& x() { return m_x; }
		Real& y() { return m_y; }

	private:
		Real m_x, m_y;
	};
}