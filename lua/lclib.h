#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <lua.h>
#include <lauxlib.h>

LUALIB_API int (luaC_loadfile)(lua_State *L, const char *filename);
LUALIB_API int (luaC_loadbuffer)(lua_State *L, const char *buff, size_t sz,
	const char *name);
LUALIB_API int (luaC_loadstring)(lua_State *L, const char *s);

LUALIB_API void (luaC_openlib)(lua_State *L);

#ifdef __cplusplus
}
#endif
