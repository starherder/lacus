#include "string_util.h"
#include <algorithm>


#ifdef _WIN32
#include <windows.h>
#else
#include <cstring>
#endif


namespace utility
{
    bool StringUtil::is_number(const std::string& s)
    {
        return !s.empty() &&
            std::all_of(s.begin(), s.end(),
                [](unsigned char c) { return std::isdigit(c); });
    }

	StrViewVector StringUtil::split(const std::string& str, char delimiter)
	{
		StrViewVector tokens;

		std::string_view sv(str);

		size_t start = 0;
		size_t end = sv.find(delimiter);

		while (end != std::string_view::npos) {
			tokens.push_back(sv.substr(start, end - start));
			start = end + 1;
			end = sv.find(delimiter, start);
		}

		tokens.push_back(sv.substr(start));
		return tokens;
	}

    namespace detail {
#ifdef _WIN32
        inline std::wstring utf8_to_wstring(const std::u8string& u8_str) {
            if (u8_str.empty()) return {};
            int len = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)u8_str.data(), (int)u8_str.size(), nullptr, 0);
            if (len <= 0) throw std::runtime_error("UTF8→WString failed: invalid encode");
            std::wstring wstr(len, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, (LPCCH)u8_str.data(), (int)u8_str.size(), &wstr[0], len);
            return wstr;
        }

        inline std::string wstring_to_ansi(const std::wstring& wstr) {
            if (wstr.empty()) return {};
            int len = WideCharToMultiByte(CP_ACP, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
            if (len <= 0) throw std::runtime_error("WString→ANSI failed: invalid encode");
            std::string ansi(len, '\0');
            WideCharToMultiByte(CP_ACP, 0, wstr.data(), (int)wstr.size(), &ansi[0], len, nullptr, nullptr);
            return ansi;
        }

        inline std::wstring ansi_to_wstring(const std::string& ansi_str) {
            if (ansi_str.empty()) return {};
            int len = MultiByteToWideChar(CP_ACP, 0, ansi_str.data(), (int)ansi_str.size(), nullptr, 0);
            if (len <= 0) throw std::runtime_error("ANSI→WString failed: invalid encode");
            std::wstring wstr(len, L'\0');
            MultiByteToWideChar(CP_ACP, 0, ansi_str.data(), (int)ansi_str.size(), &wstr[0], len);
            return wstr;
        }

        inline std::u8string wstring_to_utf8(const std::wstring& wstr) {
            if (wstr.empty()) return {};
            int len = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
            if (len <= 0) throw std::runtime_error("WString→UTF8 failed: invalid encode");
            std::u8string u8_str(len, u8'\0');
            WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), (LPSTR)u8_str.data(), len, nullptr, nullptr);
            return u8_str;
        }

#else
        inline std::string utf8_to_ansi(const std::u8string& u8_str) {
            return std::string((const char*)u8_str.data(), u8_str.size());
        }

        inline std::u8string ansi_to_utf8(const std::string& ansi_str) {
            return std::u8string((const char8_t*)ansi_str.data(), ansi_str.size());
        }
#endif
    } // namespace detail

    std::string StringUtil::utf8_to_ansi(const std::u8string& u8_str) {
        try {
#ifdef _WIN32
            auto wstr = detail::utf8_to_wstring(u8_str);
            return detail::wstring_to_ansi(wstr);
#else
            return detail::utf8_to_ansi(u8_str);
#endif
        } catch (...) { 
            throw std::runtime_error("UTF8→ANSI convert failed"); 
        }
    }

    std::u8string StringUtil::ansi_to_utf8(const std::string& ansi_str) {
        try {
#ifdef _WIN32
            auto wstr = detail::ansi_to_wstring(ansi_str);
            return detail::wstring_to_utf8(wstr);
#else
            return detail::ansi_to_utf8(ansi_str);
#endif
        } catch (...) { 
            throw std::runtime_error("ANSI→UTF8 convert failed"); 
        }
    }

    std::string StringUtil::utf8_str_to_ansi(const std::string& utf8_str) {
        return utf8_to_ansi(std::u8string((const char8_t*)utf8_str.data(), utf8_str.size()));
    }

    std::string StringUtil::ansi_to_utf8_str(const std::string& ansi_str) {
        auto u8 = ansi_to_utf8(ansi_str);
        return std::string((const char*)u8.data(), u8.size());
    }









}