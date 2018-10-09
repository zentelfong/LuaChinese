#include "lconvert.h"


struct stRegHelper {
	const TCHAR*	cn;
	const TCHAR*	en;
};


const stRegHelper key_words[] = {
	{ T("���"), T("if") },
	{ T("��ô"), T("then") },
	{ T("����"), T("end") },
	{ T("����"), T("else") },
	{ T("�������"), T("elseif") },
	{ T("����"), T("elseif") },
	{ T("ѭ��"), T("while") },
	{ T("�ֲ�����"), T("local") },
	{ T("����"), T("return") },
	{ T("��"), T("nil") },
	{ T("��"), T("true") },
	{ T("��"), T("false") },
	{ T("��"), T("not") },
	{ T("û��"), T("not") },
	{ T("û"), T("not") },
	{ T("ִ��"), T("do") },
	{ T("��"), T("and") },
	{ T("��"), T("or") },
	{ T("���庯��"), T("function") },
	{ T("�ظ�"), T("repeat") },
	{ T("ֱ��"), T("until") },
	{ T("����"), T("break") },
	{ T("����"), T("for") },
	{ NULL,NULL }
};

int TextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

int TextStrToUtf8(char* buf, int buf_size, const wchar_t* in_text, const wchar_t* in_text_end)
{
	char* buf_out = buf;
	const char* buf_end = buf + buf_size;
	while (buf_out < buf_end - 1 && (!in_text_end || in_text < in_text_end) && *in_text)
	{
		unsigned int c = (unsigned int)(*in_text++);
		if (c < 0x80)
			*buf_out++ = (char)c;
		else
			buf_out += TextCharToUtf8(buf_out, (int)(buf_end - buf_out - 1), c);
	}
	*buf_out = 0;
	return (int)(buf_out - buf);
}

LConvert::LConvert()
{
	const stRegHelper* keywords = key_words;
	while (keywords->cn)
	{
		std::string key,value;
#ifdef UNICODE
		char buf[64];
		TextStrToUtf8(buf,sizeof(buf), keywords->cn,NULL);
		key = buf;
		TextStrToUtf8(buf, sizeof(buf), keywords->en, NULL);
		value = buf;
#else
		key = keywords->cn;
		value = keywords->en;
#endif
		m_keyWords[key] = value;
		++keywords;
	}
}


LConvert::~LConvert()
{


}

bool LConvert::Convert(const char* in, std::string& out)
{
	const char* ptr = in;
	std::string keyWord;
	if (!in)
		return false;

	while (*ptr)
	{
		switch (*ptr)
		{
			case '\n':
			case '\r':
			case '\t':
			case ' ':
			case '(':
			case ')':
			case ';':
			case ',':
			{
				if (!keyWord.empty())
				{
					map_t::iterator find = m_keyWords.find(keyWord);
					if (find != m_keyWords.end())
					{
						out.append(find->second);
					}
					else
						out.append(keyWord);

					keyWord.clear();
				}
				out.push_back(*ptr);
				break;
			}
			default:
			{
				bool writeToBuf = false;
				switch (*ptr)
				{
				case '\'':
				{
					keyWord.push_back(*ptr);
					++ptr;

					//�ҵ������Ž���
					while (*ptr && *ptr != '\'')
					{
						if (*ptr == '\\' && ptr[1] == '\'')
						{
							// \'
							keyWord.push_back(*ptr);
							++ptr;
						}
						keyWord.push_back(*ptr);
						++ptr;
					}
					writeToBuf = true;
					break;
				}

				case '\"':
				{
					keyWord.push_back(*ptr);
					++ptr;

					//�ҵ�˫���Ž���
					while (*ptr && *ptr != '\"')
					{
						if (*ptr == '\\' && ptr[1] == '\"')
						{
							// \"
							keyWord.push_back(*ptr);
							++ptr;
						}
						keyWord.push_back(*ptr);
						++ptr;
					}
					writeToBuf = true;
					break;
				}
				case '[':
				{
					if (ptr[1]=='[')
					{
						//[[]]
						keyWord.push_back(*ptr);++ptr;
						keyWord.push_back(*ptr);++ptr;

						//
						while (*ptr)
						{
							if (*ptr == ']' && ptr[1] == ']')
							{
								// \"
								keyWord.push_back(*ptr);++ptr;
								break;
							}
							keyWord.push_back(*ptr);
							++ptr;
						}

					}
					else if (ptr[1] == '=' && ptr[2] == '=' && ptr[3] == '[')
					{
						//[==[]==]
						keyWord.push_back(*ptr); ++ptr;
						keyWord.push_back(*ptr); ++ptr;
						keyWord.push_back(*ptr); ++ptr;
						keyWord.push_back(*ptr); ++ptr;

						while (*ptr)
						{
							if (*ptr == ']' && ptr[1] == '=' && ptr[2] == '=' && ptr[3] == ']')
							{
								keyWord.push_back(*ptr);++ptr;
								keyWord.push_back(*ptr); ++ptr;
								keyWord.push_back(*ptr); ++ptr;
								break;
							}
							keyWord.push_back(*ptr);
							++ptr;
						}
					}
					writeToBuf = true;
					break;
				}
				default:
					break;
				}

				keyWord.push_back(*ptr);
				if (writeToBuf)
				{
					out.append(keyWord);
					keyWord.clear();
				}
			}
		}
		++ptr;
	}

	if (!keyWord.empty())
	{
		map_t::iterator find = m_keyWords.find(keyWord);
		if (find != m_keyWords.end())
		{
			out.append(find->second);
		}
		else
			out.append(keyWord);
	}

	return true;
}


