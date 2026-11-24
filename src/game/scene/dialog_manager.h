#pragma once

#include "game/scene/game_context.h"

#include "utility/i_singleton.h"

namespace game
{
	struct DialogLine
	{
		std::string actor;
		std::string emotion;
		std::string word;
		std::string avatar;
		int duration = 0;
	};

	struct DialogConfig
	{
		std::string name;
		std::string type;
		std::string desc;
		bool camera_follow = false;
		bool game_pause = false;
		std::vector<DialogLine> dialogue;
	};

	class DialogManager final : public utility::ISingleton<DialogManager>
	{
	public:
		using DialogConfigMap = std::map<std::string, DialogConfig>;

	public:
		DialogManager() = default;
		~DialogManager() = default;

		DialogManager(DialogManager&&) = delete;
		DialogManager(const DialogManager&) = delete;

		void init(GameContext* context);
		bool load(const fs::path& dialogdir);
		bool loadDialogFile(const fs::path& dialogfile);

		const DialogConfig* getDialog(const std::string& name) const;
		const DialogConfigMap& getDialogs() const { return _dialogs; }

	private:
		int parseDuration(const nJson& item) const;
		bool parseBool(const nJson& item, const std::string& key, bool defaultValue = false) const;
		std::string parseAvatar(const nJson& item, const std::string& actor) const;

	private:
		GameContext* _context = nullptr;
		DialogConfigMap _dialogs;
		fs::path _dialogPath;
	};
}
