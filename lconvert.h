#pragma once
#include <string>
#include <vector>
#include <map>

#ifdef UNICODE
#define T(x) L##x
#define TCHAR wchar_t
#else
#define T(x) x
#define TCHAR char
#endif


//标识符转换器
class LConvert
{
public:
	LConvert();
	~LConvert();

	bool Convert(const char* in, std::string& out);

private:
	typedef std::map<std::string, std::string> map_t;
	map_t m_keyWords;
};



