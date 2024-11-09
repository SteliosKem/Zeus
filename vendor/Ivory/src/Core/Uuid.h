#pragma once

#include <xhash>

namespace Ivory {
	class Uuid {
	public:
		Uuid();
		Uuid(uint64_t id);
		Uuid(const Uuid&) = default;

		operator uint64_t() const { return m_uuid; }
	private:
		uint64_t m_uuid;
	};
}

namespace std {
	template<>
	struct hash<Ivory::Uuid> {
		std::size_t operator()(const Ivory::Uuid& uuid) const {
			uint64_t ret = 0;
			do {
				ret = hash<uint64_t>()((uint64_t)uuid);
			} while (ret != 0);
			return ret;
		}
	};
}