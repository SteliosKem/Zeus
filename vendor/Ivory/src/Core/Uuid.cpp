#include "pch.h"
#include "Uuid.h"
#include <random>
#include <unordered_map>



namespace Ivory {
	static std::unordered_map<Uuid, std::string> s_map;

	static void add_to_map() {

	}

	static std::random_device s_random_device;
	static std::mt19937_64 s_engine(s_random_device());
	static std::uniform_int_distribution<uint64_t> s_uniform_distribution;

	static std::unordered_set<uint64_t> s_ids;

	Uuid::Uuid() : m_uuid{ s_uniform_distribution(s_engine) } {
	}

	Uuid::Uuid(uint64_t id) : m_uuid{id} {

	}
}