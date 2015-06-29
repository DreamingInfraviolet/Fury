#include <codecvt>
#include <vector>
#include <cctype>

std::wstring stringToWstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

std::string stringFromStrLen(const char* str, size_t length)
{
	std::vector<char> tmp;
	tmp.resize(length + 1);
	strncpy(&tmp[0], str, length);
	tmp.back() = '\0';
	return std::string(&tmp[0]);
}

std::string toLowerCase(const std::string& str)
{
	std::string out = str;
	for (size_t i = 0; i < str.size(); ++i)
		out[i] = (char)std::tolower(str[i]);
	return out;
}