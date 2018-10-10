#include "lclib.h"
#include <stdio.h>
#include <string.h>
#include "lconvert.h"

static LConvert lc_convert;

typedef struct LCLoadF {
	int extraline;
	FILE *f;
	char buff[LUAL_BUFFERSIZE];
} LCLoadF;


static const char * lcgetF(lua_State *L, void *ud, size_t *size) {
	LCLoadF *lf = (LCLoadF *)ud;
	(void)L;
	if (lf->extraline) {
		lf->extraline = 0;
		*size = 1;
		return "\n";
	}
	if (feof(lf->f)) return NULL;
	*size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);
	return (*size > 0) ? lf->buff : NULL;
}


static int lcerrfile(lua_State *L, const char *what, int fnameindex) {
	const char *serr = strerror(errno);
	const char *filename = lua_tostring(L, fnameindex) + 1;
	lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
	lua_remove(L, fnameindex);
	return LUA_ERRFILE;
}


LUALIB_API int luaC_loadfile(lua_State *L, const char *filename) {
	LCLoadF lf;
	int status, readstatus;
	int c;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	bool needConvert = true;
	lf.extraline = 0;
	if (filename == NULL) {
		lua_pushliteral(L, "=stdin");
		lf.f = stdin;
	}
	else {
		lua_pushfstring(L, "@%s", filename);
		lf.f = fopen(filename, "r");
		if (lf.f == NULL) return lcerrfile(L, "open", fnameindex);
	}
	c = getc(lf.f);
	if (c == '#') {  /* Unix exec. file? */
		lf.extraline = 1;
		while ((c = getc(lf.f)) != EOF && c != '\n');  /* skip first line */
		if (c == '\n') c = getc(lf.f);
	}
	if (c == LUA_SIGNATURE[0] && filename) {  /* binary file? */
		lf.f = freopen(filename, "rb", lf.f);  /* reopen in binary mode */
		if (lf.f == NULL) return lcerrfile(L, "reopen", fnameindex);
		/* skip eventual `#!...' */
		while ((c = getc(lf.f)) != EOF && c != LUA_SIGNATURE[0]);
		lf.extraline = 0;
		needConvert = false;
	}
	ungetc(c, lf.f);

	if (needConvert)
	{
		//需要转换，直接读取全部内容加载
		std::string data;
		while (!feof(lf.f))
		{
			size_t size = fread(lf.buff, 1, sizeof(lf.buff), lf.f);
			if (size)
			{
				data.append(lf.buff, size);
			}
		}
		std::string dataOut;
		lc_convert.Convert(data.c_str(), dataOut);
		status = luaL_loadbuffer(L, dataOut.c_str(), dataOut.length(), lua_tostring(L, -1));
	}
	else
		status = lua_load(L, lcgetF, &lf, lua_tostring(L, -1));

	readstatus = ferror(lf.f);
	if (filename) fclose(lf.f);  /* close file (even in case of errors) */
	if (readstatus) {
		lua_settop(L, fnameindex);  /* ignore results from `lua_load' */
		return lcerrfile(L, "read", fnameindex);
	}
	lua_remove(L, fnameindex);
	return status;
}



LUALIB_API int luaC_loadbuffer(lua_State *L, const char *buff, size_t size, const char *name)
{
	if (buff[0] == LUA_SIGNATURE[0])
		return luaL_loadbuffer(L, buff, size, name);

	std::string data(buff,size);
	std::string dataOut;
	lc_convert.Convert(data.c_str(), dataOut);
	return luaL_loadbuffer(L, dataOut.c_str(), dataOut.length(), name);
}


LUALIB_API int luaC_loadstring(lua_State *L, const char *s)
{
	std::string dataOut;
	lc_convert.Convert(s, dataOut);
	return luaL_loadbuffer(L, dataOut.c_str(), dataOut.length(), s);
}

static const char *lcgeneric_reader(lua_State *L, void *ud, size_t *size) {
	(void)ud;  /* to avoid warnings */
	luaL_checkstack(L, 2, "too many nested functions");
	lua_pushvalue(L, 1);  /* get function */
	lua_call(L, 0, 1);  /* call it */
	if (lua_isnil(L, -1)) {
		*size = 0;
		return NULL;
	}
	else if (lua_isstring(L, -1)) {
		const char* str = lua_tostring(L, -1);
		if (str)
		{
			//转换脚本
			std::string dataOut;
			lc_convert.Convert(str, dataOut);
			lua_pop(L, 1);
			lua_pushlstring(L, dataOut.c_str(), dataOut.length());
		}
		lua_replace(L, 3);  /* save string in a reserved stack slot */
		return lua_tolstring(L, 3, size);
	}
	else luaL_error(L, "reader function must return a string");
	return NULL;  /* to avoid warnings */
}


LUALIB_API int luaC_load(lua_State *L) {
	int status;
	const char *cname = luaL_optstring(L, 2, "=(load)");
	luaL_checktype(L, 1, LUA_TFUNCTION);
	lua_settop(L, 3);  /* function, eventual name, plus one reserved slot */
	status = lua_load(L, lcgeneric_reader, NULL, cname);

	if (status == 0)  /* OK? */
		return 1;
	else {
		lua_pushnil(L);
		lua_insert(L, -2);  /* put before error message */
		return 2;  /* return nil plus error message */
	}
}








extern int TextStrToUtf8(char* buf, int buf_size, const wchar_t* in_text, const wchar_t* in_text_end);

const TCHAR *lclibScript = T("垃圾回收 = collectgarbage;" \
	"断言 = assert;" \
	"执行文件 = dofile;" \
	"错误 = error;" \
	"获取环境 = getfenv;" \
	"获取元表 = getmetatable;" \
	"数组迭代 = ipairs;" \
	"迭代 = pairs;" \
	"加载 = load;" \
	"加载文件 = loadfile;" \
	"加载字符串 = loadstring;" \
	"模块定义 = module;" \
	"下一个 = next;" \
	"调用 = pcall;" \
	"打印 = print;" \
	"原始相等 = rawequal;" \
	"原始获取 = rawget;" \
	"原始设置 = rawset;" \
	"包含 = require;" \
	"选择 = select;" \
	"设置环境 = setfenv;" \
	"设置元表 = setmetatable;" \
	"转为数字 = tonumber;" \
	"转为字符串 = tostring;" \
	"类型 = type;" \
	"拆开 = unpack;" \
	"保护调用 = xpcall;" \
	"_全局 = _G;" \
	"_环境 = _ENV;"
);

LUALIB_API void luaC_openlib(lua_State *L)
{
#ifdef UNICODE
	char buf[1024];
	TextStrToUtf8(buf, sizeof(buf), lclibScript, NULL);
	luaL_dostring(L, buf);
#else
	luaL_dostring(L, lclibScript);
#endif
}