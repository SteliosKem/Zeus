#pragma once

#ifdef IV_PLATFORM_WINDOWS
/*#ifdef IV_BUILD_DLL
		#define	IVORY_API __declspec(dllexport)
	#else
		#define IVORY_API __declspec(dllimport)
	#endif*/
#define IVORY_API
#endif

#ifdef IV_ASSERTS
	#define	IV_ASSERT(x, ...) { if(!(x)) {IV_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define	IV_CARE_ASSERT(x, ...) { if(!(x)) {IV_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define IV_ASSERT(x, ...)
	#define IV_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define IV_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

#define GLM_FORCE_CTOR_INIT

#define YAML_CPP_API

using Real = float;

// Uncomment to compile with double precision
//using Real = double;