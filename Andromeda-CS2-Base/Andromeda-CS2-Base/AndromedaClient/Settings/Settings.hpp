#pragma once

#include <Common/Common.hpp>
#include <ImGui/imgui.h>

namespace Settings
{
	namespace Visual
	{
		inline auto Active = true;
		inline auto Team = true;
		inline auto Enemy = true;
		inline auto OnlyVisible = false;
		inline auto PlayerBox = true;
		inline auto BoneESP = true;
		inline auto BoneESPTeam = true;
		inline auto BoneESPEnemy = true;
		inline auto Glow = true;
		inline auto GlowTeam = true;
		inline auto GlowEnemy = true;

		inline auto PlayerBoxType = 3;
	}
	namespace Menu
	{
		inline auto MenuAlpha = 200;
		inline auto MenuStyle = 0;
	}
	namespace Colors
	{
		namespace Visual
		{
			inline ImVec4 PlayerBoxTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 PlayerBoxTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 PlayerBoxCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 PlayerBoxCT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 BoneESPTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 BoneESPTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 BoneESPCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 BoneESPCT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 GlowTT = { 255.f / 255.f , 75.f / 255.f , 75.f / 255.f  , 1.f };
			inline ImVec4 GlowTT_Visible = { 0 , 1.f , 0.f , 1.f };
			inline ImVec4 GlowCT = { 65.f / 255.f , 200 / 255.f , 255.f / 255.f , 1.f };
			inline ImVec4 GlowCT_Visible = { 0 , 1.f , 0.f , 1.f };
		}
	}
	
	namespace Aimbot
	{
		inline auto bEnabled = false;
		inline auto fFOV = 5.0f;
		inline auto bSmoothing = true;
		inline auto fSmoothFactor = 5.0f;
		inline auto iAimBone = 0; // 0 = head, 1 = chest, 2 = pelvis
		inline auto bTeamCheck = true;
		inline auto bVisibleCheck = true;
		inline auto bFOVCircle = true;
		inline auto fFOVCircleRadius = 50.0f;
		inline auto vFOVCircleColor = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);
	}

	namespace RageBot
	{
		inline auto bEnabled       = false;
		inline auto bAutoShoot     = true;
		inline auto bAutoStop      = true;
		inline auto bAutoScope     = true;
		inline auto fFOV           = 180.0f;
		inline auto iAimBone       = 0;
		inline auto bTeamCheck     = true;
		inline auto bVisibleCheck  = false;
		inline auto fMinDamage     = 1.0f;
		inline auto bHitchance     = true;   // включить hitchance
		inline auto fMinHitchance  = 75.0f;  // минимальный hitchance для выстрела (0-100%)
		inline auto bSilentAim     = false;  // наведение без изменения углов
		inline auto bAntiAim       = false;  // развороты для защиты
		inline auto iAntiAimType   = 0;      // 0=off, 1=jitter, 2=spin
		inline auto bThirdperson   = false;  // камера от третьего лица
		inline auto bBacktrack     = false;
		inline auto iBacktrackTick = 12;
	}
}
