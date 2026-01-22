#pragma once

#include "i_singleton.h"
#include "string_util.h"

#include <map>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>

namespace utility 
{
	enum class Language 
    {
		SimpleChinese,
		TraditionalChinese,
		English,
		Other,
	};

	class StringTranslator : public ISingleton<StringTranslator>
	{
	public:

		using Translator = std::map<std::string, std::string>;
		using AllLanguage = std::map<Language, Translator>;

	public:

		Language getLanguage() { return _language; }

		bool setLanguage(Language lan) { _language = lan; }

        bool load(Language language, const std::filesystem::path& dir)
        {
            for (const auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (entry.is_regular_file())
                {
                    auto filename = entry.path();
                    loadfile(language, filename);
                }
            }
            return true;
        }

		bool loadfile(Language language, const std::filesystem::path& file_path)
		{
            auto trim_space = [](std::string& s) {
                s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
                s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), s.end());
            };

            std::ifstream ifs(file_path, std::ios::in);
            if (!ifs.is_open())
            {
                return false;
            }

            auto& text = _all_text[language];

            std::string line;
            std::size_t line_num = 0;
            while (std::getline(ifs, line)) {
                line_num++;

                trim_space(line);

                if (line.empty() || line.starts_with('#') || line.starts_with("//")) {
                    continue;
                }

                const std::size_t eq_pos = line.find('=');
                if (eq_pos == 0 || eq_pos == line.size() - 1 || eq_pos == std::string::npos) {
                    continue;
                }

                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);

                trim_space(key);
                trim_space(value);

                text.emplace(std::move(key), std::move(value));
            }

            if (!ifs.eof()) {
                throw std::runtime_error("read file failed. eof NOT found.");
            }

            ifs.close();
			return true;
		}

		std::string getAnsiString(const std::string& key)
		{
            return StringUtil::utf8_str_to_ansi(getUtf8String(key));
		}

		std::string getUtf8String(const std::string& key)
		{
            auto& text = _all_text[_language];
            auto it = text.find(key);
            if (it == text.end()) {
                return key;
            }

            return it->second;
		}

	private:
		Language _language = Language::SimpleChinese;

		AllLanguage _all_text;
	};


}

#define Trans(key) utility::StringTranslator::inst().getUtf8String(key)