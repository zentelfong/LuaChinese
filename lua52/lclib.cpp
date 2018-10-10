#include "lclib.h"
#include <stdio.h>
#include <string.h>
#include "lconvert.h"

static LConvert lc_convert;

typedef struct LCLoadF {
	int n;  /* number of pre-read characters */
	FILE *f;  /* file being read */
	char buff[BUFSIZ];  /* area for reading file */
} LCLoadF;


static const char *lcgetF(lua_State *L, void *ud, size_t *size) {
	LCLoadF *lf = (LCLoadF *)ud;
	(void)L;  /* not used */
	if (lf->n > 0) {  /* are there pre-read characters to be read? */
		*size = lf->n;  /* return them (chars already in buffer) */
		lf->n = 0;  /* no more pre-read characters */
	}
	else {  /* read a block from file */
			/* 'fread' can return > 0 *and* set the EOF flag. If next call to
			'getF' called 'fread', it might still wait for user input.
			The next check avoids this problem. */
		if (feof(lf->f)) return NULL;
		*size = fread(lf->buff, 1, sizeof(lf->buff), lf->f);  /* read block */
	}
	return lf->buff;
}



static int lcerrfile(lua_State *L, const char *what, int fnameindex) {
	const char *serr = strerror(errno);
	const char *filename = lua_tostring(L, fnameindex) + 1;
	lua_pushfstring(L, "cannot %s %s: %s", what, filename, serr);
	lua_remove(L, fnameindex);
	return LUA_ERRFILE;
}

static int lcskipBOM(LCLoadF *lf) {
	const char *p = "\xEF\xBB\xBF";  /* UTF-8 BOM mark */
	int c;
	lf->n = 0;
	do {
		c = getc(lf->f);
		if (c == EOF || c != *(const unsigned char *)p++) return c;
		lf->buff[lf->n++] = c;  /* to be read by the parser */
	} while (*p != '\0');
	lf->n = 0;  /* prefix matched; discard it */
	return getc(lf->f);  /* return next character */
}

static int lcskipcomment(LCLoadF *lf, int *cp) {
	int c = *cp = lcskipBOM(lf);
	if (c == '#') {  /* first line is a comment (Unix exec. file)? */
		do {  /* skip first line */
			c = getc(lf->f);
		} while (c != EOF && c != '\n');
		*cp = getc(lf->f);  /* skip end-of-line, if present */
		return 1;  /* there was a comment */
	}
	else return 0;  /* no comment */
}


LUALIB_API int luaC_loadfilex(lua_State *L, const char *filename,
	const char *mode) {
	LCLoadF lf;
	int status, readstatus;
	int c;
	int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
	bool needConvert = true;
	if (filename == NULL) {
		lua_pushliteral(L, "=stdin");
		lf.f = stdin;
	}
	else {
		lua_pushfstring(L, "@%s", filename);
		lf.f = fopen(filename, "r");
		if (lf.f == NULL) return lcerrfile(L, "open", fnameindex);
	}
	if (lcskipcomment(&lf, &c))  /* read initial portion */
		lf.buff[lf.n++] = '\n';  /* add line to correct line numbers */
	if (c == LUA_SIGNATURE[0] && filename) {  /* binary file? */
		lf.f = freopen(filename, "rb", lf.f);  /* reopen in binary mode */
		if (lf.f == NULL) return lcerrfile(L, "reopen", fnameindex);
		lcskipcomment(&lf, &c);  /* re-read initial portion */
		needConvert = false;
	}
	if (c != EOF)
		lf.buff[lf.n++] = c;  /* 'c' is the first character of the stream */
	
	if (needConvert)
	{
		//需要转换，直接读取全部内容加载
		std::string data;
		for (int i = 0; i<lf.n; ++i)
			data.push_back(lf.buff[i]);
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
		status = luaL_loadbufferx(L, dataOut.c_str(), dataOut.length(), lua_tostring(L, -1), mode);
	}
	else
		status = lua_load(L, lcgetF, &lf, lua_tostring(L, -1), mode);

	readstatus = ferror(lf.f);
	if (filename) fclose(lf.f);  /* close file (even in case of errors) */
	if (readstatus) {
		lua_settop(L, fnameindex);  /* ignore results from 'lua_load' */
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


/////////////////////////////////////////////////////////////////////////////////////////

static int lcload_aux(lua_State *L, int status, int envidx) {
	if (status == LUA_OK) {
		if (envidx != 0) {  /* 'env' parameter? */
			lua_pushvalue(L, envidx);  /* environment for loaded function */
			if (!lua_setupvalue(L, -2, 1))  /* set it as 1st upvalue */
				lua_pop(L, 1);  /* remove 'env' if not used by previous call */
		}
		return 1;
	}
	else {  /* error (message is on top of the stack) */
		lua_pushnil(L);
		lua_insert(L, -2);  /* put before error message */
		return 2;  /* return nil plus error message */
	}
}

#define RESERVEDSLOT	5

static const char *lcgeneric_reader(lua_State *L, void *ud, size_t *size) {
	(void)(ud);  /* not used */
	luaL_checkstack(L, 2, "too many nested functions");
	lua_pushvalue(L, 1);  /* get function */
	lua_call(L, 0, 1);  /* call it */
	if (lua_isnil(L, -1)) {
		lua_pop(L, 1);  /* pop result */
		*size = 0;
		return NULL;
	}
	else if (!lua_isstring(L, -1))
		luaL_error(L, "reader function must return a string");

	else {
		const char* str = lua_tostring(L, -1);
		std::string dataOut;
		lc_convert.Convert(str, dataOut);
		lua_pop(L, 1);
		lua_pushlstring(L, dataOut.c_str(), dataOut.length());
	}

	lua_replace(L, RESERVEDSLOT);  /* save string in reserved slot */
	return lua_tolstring(L, RESERVEDSLOT, size);
}

int luaC_load(lua_State *L) {
	int status;
	size_t l;
	const char *s = lua_tolstring(L, 1, &l);
	const char *mode = luaL_optstring(L, 3, "bt");
	int env = (!lua_isnone(L, 4) ? 4 : 0);  /* 'env' index or 0 if no 'env' */
	if (s != NULL) {  /* loading a string? */
		const char *chunkname = luaL_optstring(L, 2, s);
		status = luaL_loadbufferx(L, s, l, chunkname, mode);
	}
	else {  /* loading from a reader function */
		const char *chunkname = luaL_optstring(L, 2, "=(load)");
		luaL_checktype(L, 1, LUA_TFUNCTION);
		lua_settop(L, RESERVEDSLOT);  /* create reserved slot */
		status = lua_load(L, lcgeneric_reader, NULL, chunkname, mode);
	}
	return lcload_aux(L, status, env);
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