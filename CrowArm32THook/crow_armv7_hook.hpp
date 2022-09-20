#pragma once

#include "../includes/crow_types.hpp"
#include "./crow_armv7_transport.hpp"

namespace crow
{
	#ifdef __cplusplus
	extern "C" {
	#endif

	enum crow_hook_status {
		
		CROW_HOOK_OK = 0,
		CROW_HOOK_ERROR_NOT_INITIALIZED,
		CROW_HOOK_ERROR_NOT_EXECUTABLE,
		CROW_HOOK_ERROR_NOT_REGISTERED,
		CROW_HOOK_ERROR_NOT_HOOKED,
		CROW_HOOK_ERROR_ALREADY_REGISTERED,
		CROW_HOOK_ERROR_ALREADY_HOOKED,
		CROW_HOOK_ERROR_SO_NOT_FOUND,
		CROW_HOOK_ERROR_FUNCTION_NOT_FOUND,
		CROW_HOOK_ERROR_UNKNOWN = 0xfffffffc
	};


	enum crow_hook_status crow_registeri_nline_hook(U32 target_addr, U32 new_addr, U32 **proto_addr);
	enum crow_hook_status inline_hook(U32 target_addr);
	void inline_un_hook_all();
	enum crow_hook_status inline_hook(U32 target_addr);
	void inline_hook_all();

	#ifdef __cplusplus
	}
	#endif
}