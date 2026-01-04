#pragma once

#include <codecvt>
#pragma once


#include <string>
#include <iostream>
#include <vector>

namespace utility
{

	class StringUtil
	{
	public:
		static std::vector<std::string_view> split(const std::string& str, char delimiter = ',');

		static std::string utf8_to_ansi(const std::u8string& u8_str);
		static std::u8string ansi_to_utf8(const std::string& ansi_str);

		static std::string utf8_str_to_ansi(const std::string& utf8_str);
		static std::string ansi_to_utf8_str(const std::string& ansi_str);
	};

}