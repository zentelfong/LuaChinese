#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "lua.h"
#include "lauxlib.h"

LUALIB_API int luaC_loadfilex(lua_State *L, const char *filename,
	const char *mode);

LUALIB_API int luaC_loadbuffer(lua_State *L, const char *buff, size_t sz,
	const char *name);

LUALIB_API int luaC_loadstring(lua_State *L, const char *s);

LUALIB_API int luaC_load(lua_State *L);

LUALIB_API void luaC_openlib(lua_State *L);

#define luaC_loadfile(L,f)	luaC_loadfilex(L,f,NULL)

#ifdef __cplusplus
}
#endif
