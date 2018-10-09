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
		//��Ҫת����ֱ�Ӷ�ȡȫ�����ݼ���
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


LUALIB_API int (luaC_loadstring)(lua_State *L, const char *s)
{
	std::string dataOut;
	lc_convert.Convert(s, dataOut);
	return luaL_loadbuffer(L, dataOut.c_str(), dataOut.length(), s);
}


extern int TextStrToUtf8(char* buf, int buf_size, const wchar_t* in_text, const wchar_t* in_text_end);

const TCHAR *lclibScript = T("�������� = collectgarbage;" \
	"���� = assert;" \
	"ִ���ļ� = dofile;" \
	"���� = error;" \
	"��ȡ���� = getfenv;" \
	"��ȡԪ�� = getmetatable;" \
	"������� = ipairs;" \
	"���� = pairs;" \
	"���� = load;" \
	"�����ļ� = loadfile;" \
	"�����ַ��� = loadstring;" \
	"ģ�鶨�� = module;" \
	"��һ�� = next;" \
	"���� = pcall;" \
	"��ӡ = print;" \
	"ԭʼ��� = rawequal;" \
	"ԭʼ��ȡ = rawget;" \
	"ԭʼ���� = rawset;" \
	"�����ļ� = require;" \
	"ѡ�� = select;" \
	"���û��� = setfenv;" \
	"����Ԫ�� = setmetatable;" \
	"תΪ���� = tonumber;" \
	"תΪ�ַ��� = tostring;" \
	"���� = type;" \
	"�� = unpack;" \
	"�������� = xpcall;"
);

LUALIB_API void (luaC_openlib)(lua_State *L)
{
#ifdef UNICODE
	char buf[1024];
	TextStrToUtf8(buf, sizeof(buf), lclibScript, NULL);
	luaL_dostring(L, buf);
#else
	luaL_dostring(L, lclibScript);
#endif
}