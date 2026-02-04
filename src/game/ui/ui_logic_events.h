#pragma once


namespace game
{
	enum SceneGameMode
	{
		GameMode_Test,
		GameMode_AutoChess,
		GameMode_TileBattle,
	};

	enum CustomEventId
	{
		Event_SelectScene,
		Event_ToggleMainForm,

		Event_ShowScenes,
		Event_ReplayLevel,

		Event_Max,
	};


}