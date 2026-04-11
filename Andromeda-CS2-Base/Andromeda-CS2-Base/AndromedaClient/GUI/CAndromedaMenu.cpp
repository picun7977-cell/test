#include "CAndromedaMenu.hpp"

#include <ImGui/imgui.h>

#include <AndromedaClient/Settings/Settings.hpp>
#include <AndromedaClient/CAndromedaGUI.hpp>

static CAndromedaMenu g_CAndromedaMenu{};

auto CAndromedaMenu::OnRenderMenu() -> void
{
	const float MenuAlpha = static_cast<float>( Settings::Menu::MenuAlpha ) / 255.f;

	ImGui::PushStyleVar( ImGuiStyleVar_Alpha , MenuAlpha );
	ImGui::SetNextWindowSize( ImVec2( 500 , 500 ) , ImGuiCond_FirstUseEver );

	if ( ImGui::Begin( XorStr( CHEAT_NAME ) , 0 ) )
	{
		if ( ImGui::CollapsingHeader( XorStr( "Aimbot" ) ) )
		{
			RenderCheckBox( XorStr( "Enabled" ) , XorStr( "##Aimbot.Enabled" ) , Settings::Aimbot::bEnabled );
			RenderSliderFloat( XorStr( "FOV" ) , XorStr( "##Aimbot.FOV" ) , Settings::Aimbot::fFOV , 1.f , 180.f );
			RenderCheckBox( XorStr( "Smoothing" ) , XorStr( "##Aimbot.Smoothing" ) , Settings::Aimbot::bSmoothing );
			if ( Settings::Aimbot::bSmoothing )
			{
				ImGui::Indent();
				RenderSliderFloat( XorStr( "  Smooth Factor" ) , XorStr( "##Aimbot.SmoothFactor" ) , Settings::Aimbot::fSmoothFactor , 1.f , 30.f );
				ImGui::Unindent();
			}

			const char* AimBoneItems[] = { "Head" , "Chest" , "Pelvis" };
			RenderComboBox( XorStr( "Aim Bone" ) , XorStr( "##Aimbot.AimBone" ) , Settings::Aimbot::iAimBone , AimBoneItems , IM_ARRAYSIZE( AimBoneItems ) );

			RenderCheckBox( XorStr( "Team Check" ) , XorStr( "##Aimbot.TeamCheck" ) , Settings::Aimbot::bTeamCheck );
			RenderCheckBox( XorStr( "Visible Check" ) , XorStr( "##Aimbot.VisibleCheck" ) , Settings::Aimbot::bVisibleCheck );
			RenderCheckBox( XorStr( "FOV Circle" ) , XorStr( "##Aimbot.FOVCircle" ) , Settings::Aimbot::bFOVCircle );
			if ( Settings::Aimbot::bFOVCircle )
			{
				ImGui::Indent();
				RenderSliderFloat( XorStr( "  FOV Circle Radius" ) , XorStr( "##Aimbot.FOVCircleRadius" ) , Settings::Aimbot::fFOVCircleRadius , 10.f , 300.f );
				ImGui::Unindent();
			}
		}

		if ( ImGui::CollapsingHeader( XorStr( "RageBot" ) ) )
		{
			RenderCheckBox( XorStr( "Enabled" ) , XorStr( "##RageBot.Enabled" ) , Settings::RageBot::bEnabled );
			RenderCheckBox( XorStr( "Auto Shoot" ) , XorStr( "##RageBot.AutoShoot" ) , Settings::RageBot::bAutoShoot );
			RenderCheckBox( XorStr( "Auto Stop" ) , XorStr( "##RageBot.AutoStop" ) , Settings::RageBot::bAutoStop );
			RenderCheckBox( XorStr( "Auto Scope" ) , XorStr( "##RageBot.AutoScope" ) , Settings::RageBot::bAutoScope );
			RenderSliderFloat( XorStr( "FOV" ) , XorStr( "##RageBot.FOV" ) , Settings::RageBot::fFOV , 1.f , 180.f );

			const char* RageBoneItems[] = { "Head" , "Chest" , "Pelvis" };
			RenderComboBox( XorStr( "Aim Bone" ) , XorStr( "##RageBot.AimBone" ) , Settings::RageBot::iAimBone , RageBoneItems , IM_ARRAYSIZE( RageBoneItems ) );

			RenderCheckBox( XorStr( "Team Check" ) , XorStr( "##RageBot.TeamCheck" ) , Settings::RageBot::bTeamCheck );
			
			RenderCheckBox( XorStr( "Hitchance" ) , XorStr( "##RageBot.Hitchance" ) , Settings::RageBot::bHitchance );
			if ( Settings::RageBot::bHitchance )
			{
				ImGui::Indent();
				RenderSliderFloat( XorStr( "  Min Hitchance" ) , XorStr( "##RageBot.MinHitchance" ) , Settings::RageBot::fMinHitchance , 0.f , 100.f );
				ImGui::Unindent();
			}
			
			RenderCheckBox( XorStr( "Silent Aim" ) , XorStr( "##RageBot.SilentAim" ) , Settings::RageBot::bSilentAim );
			RenderCheckBox( XorStr( "Anti-Aim" ) , XorStr( "##RageBot.AntiAim" ) , Settings::RageBot::bAntiAim );
			if ( Settings::RageBot::bAntiAim )
			{
				ImGui::Indent();
				const char* AntiAimItems[] = { "Off" , "Jitter" , "Spin" };
				RenderComboBox( XorStr( "  Type" ) , XorStr( "##RageBot.AntiAimType" ) , Settings::RageBot::iAntiAimType , AntiAimItems , IM_ARRAYSIZE( AntiAimItems ) );
				ImGui::Unindent();
			}
			RenderCheckBox( XorStr( "Thirdperson" ) , XorStr( "##RageBot.Thirdperson" ) , Settings::RageBot::bThirdperson );
			
			RenderCheckBox( XorStr( "Backtrack" ) , XorStr( "##RageBot.Backtrack" ) , Settings::RageBot::bBacktrack );
			if ( Settings::RageBot::bBacktrack )
			{
				ImGui::Indent();
				RenderSliderInt( XorStr( "  Backtrack Ticks" ) , XorStr( "##RageBot.BacktrackTick" ) , Settings::RageBot::iBacktrackTick , 1 , 16 );
				ImGui::Unindent();
			}
		}

		if ( ImGui::CollapsingHeader( XorStr( "Visuals" ) ) )
		{
			RenderCheckBox( XorStr( "Active" ) , XorStr( "##Visual.Active" ) , Settings::Visual::Active );
			RenderCheckBox( XorStr( "Team" ) , XorStr( "##Visual.Team" ) , Settings::Visual::Team );
			RenderCheckBox( XorStr( "Enemy" ) , XorStr( "##Visual.Enemy" ) , Settings::Visual::Enemy );
			RenderCheckBox( XorStr( "OnlyVisible" ) , XorStr( "##Visual.OnlyVisible" ) , Settings::Visual::OnlyVisible );

			RenderCheckBox( XorStr( "Player Box" ) , XorStr( "##Visual.PlayerBox" ) , Settings::Visual::PlayerBox );

			const char* PlayerBoxTypeItems[] = 
			{ 
				"Box" , "Outline Box" , "Coal Box" , "Outline Coal Box" 
			};

			RenderComboBox( XorStr( "PlayerBox Type" ) , XorStr( "##Visual.PlayerBoxType" ) , Settings::Visual::PlayerBoxType , PlayerBoxTypeItems , IM_ARRAYSIZE( PlayerBoxTypeItems ) );

			RenderCheckBox( XorStr( "Bone ESP" ) , XorStr( "##Visual.BoneESP" ) , Settings::Visual::BoneESP );
			if ( Settings::Visual::BoneESP )
			{
				ImGui::Indent();
				RenderCheckBox( XorStr( "  Bone ESP Team" ) , XorStr( "##Visual.BoneESPTeam" ) , Settings::Visual::BoneESPTeam );
				RenderCheckBox( XorStr( "  Bone ESP Enemy" ) , XorStr( "##Visual.BoneESPEnemy" ) , Settings::Visual::BoneESPEnemy );
				ImGui::Unindent();
			}

			RenderCheckBox( XorStr( "Glow" ) , XorStr( "##Visual.Glow" ) , Settings::Visual::Glow );
			if ( Settings::Visual::Glow )
			{
				ImGui::Indent();
				RenderCheckBox( XorStr( "  Glow Team" ) , XorStr( "##Visual.GlowTeam" ) , Settings::Visual::GlowTeam );
				RenderCheckBox( XorStr( "  Glow Enemy" ) , XorStr( "##Visual.GlowEnemy" ) , Settings::Visual::GlowEnemy );
				ImGui::Unindent();
			}
		}

		if ( ImGui::CollapsingHeader( XorStr( "Colors" ) ) )
		{
			RenderColorEdit( XorStr( "Player Box TT" ) , XorStr( "##Colors.Visual.PlayerBoxTT" ) , &Settings::Colors::Visual::PlayerBoxTT.x );
			RenderColorEdit( XorStr( "Player Box TT Visible" ) , XorStr( "##Colors.Visual.PlayerBoxTT_Visible" ) , &Settings::Colors::Visual::PlayerBoxTT_Visible.x );
			RenderColorEdit( XorStr( "Player Box CT" ) , XorStr( "##Colors.Visual.PlayerBoxCT" ) , &Settings::Colors::Visual::PlayerBoxCT.x );
			RenderColorEdit( XorStr( "Player Box CT Visible" ) , XorStr( "##Colors.Visual.PlayerBoxCT_Visible" ) , &Settings::Colors::Visual::PlayerBoxCT_Visible.x );

			RenderColorEdit( XorStr( "Bone ESP TT" ) , XorStr( "##Colors.Visual.BoneESPTT" ) , &Settings::Colors::Visual::BoneESPTT.x );
			RenderColorEdit( XorStr( "Bone ESP TT Visible" ) , XorStr( "##Colors.Visual.BoneESPTT_Visible" ) , &Settings::Colors::Visual::BoneESPTT_Visible.x );
			RenderColorEdit( XorStr( "Bone ESP CT" ) , XorStr( "##Colors.Visual.BoneESPCT" ) , &Settings::Colors::Visual::BoneESPCT.x );
			RenderColorEdit( XorStr( "Bone ESP CT Visible" ) , XorStr( "##Colors.Visual.BoneESPCT_Visible" ) , &Settings::Colors::Visual::BoneESPCT_Visible.x );

			RenderColorEdit( XorStr( "Glow TT" ) , XorStr( "##Colors.Visual.GlowTT" ) , &Settings::Colors::Visual::GlowTT.x );
			RenderColorEdit( XorStr( "Glow TT Visible" ) , XorStr( "##Colors.Visual.GlowTT_Visible" ) , &Settings::Colors::Visual::GlowTT_Visible.x );
			RenderColorEdit( XorStr( "Glow CT" ) , XorStr( "##Colors.Visual.GlowCT" ) , &Settings::Colors::Visual::GlowCT.x );
			RenderColorEdit( XorStr( "Glow CT Visible" ) , XorStr( "##Colors.Visual.GlowCT_Visible" ) , &Settings::Colors::Visual::GlowCT_Visible.x );
		}

		if ( ImGui::CollapsingHeader( XorStr( "Menu" ) ) )
		{
			RenderSliderInt( XorStr( "Menu Alpha" ) , XorStr( "##Menu.MenuAlpha" ) , Settings::Menu::MenuAlpha , 100 , 255 );

			const char* MenuStyleItems[] =
			{
				"Indigo" , "Vermillion" , "Classic Steam"
			};

			if ( RenderComboBox( XorStr( "Menu Style" ) , XorStr( "##Menu.MenuStyle" ) , Settings::Menu::MenuStyle , MenuStyleItems , IM_ARRAYSIZE( MenuStyleItems ) ) )
				GetAndromedaGUI()->UpdateStyle();
		}
	}

	ImGui::End();

	ImGui::PopStyleVar();
}

auto CAndromedaMenu::RenderCheckBox( const char* szTitle , const char* szStrID , bool& SettingsItem ) -> bool
{
	if ( szTitle )
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text( szTitle );
		ImGui::SameLine( ImGui::CalcTextSize( szTitle ).x + 10.f );
	}

	const auto LeftPadding = ImGui::GetStyle().FramePadding.x;

	ImGui::Dummy( ImVec2( ImGui::GetContentRegionAvail().x - 27.f - LeftPadding , 0.f ) );
	ImGui::SameLine();

	const auto Ret = ImGui::Checkbox( szStrID , &SettingsItem );

	return Ret;
}

auto CAndromedaMenu::RenderComboBox( const char* szTitle , const char* szStrID , int& v , const char* Items[] , int ItemsCount ) -> bool
{
	if ( szTitle )
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text( szTitle );
	}

	ImGui::SameLine();

	ImGui::PushItemWidth( -1.f );
	const auto Ret = ImGui::Combo( szStrID , &v , Items , ItemsCount );
	ImGui::PopItemWidth();

	return Ret;
}

auto CAndromedaMenu::RenderColorEdit( const char* szTitle , const char* szStrID , float* Color ) -> bool
{
	if ( szTitle )
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text( szTitle );
	}

	ImGui::SameLine();

	const auto Ret = ImGui::ColorEdit4( szStrID , Color );

	return Ret;
}

auto CAndromedaMenu::RenderSliderInt( const char* szTitle , const char* szStrID , int& Value , int Min , int Max ) -> bool
{
	if ( szTitle )
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text( szTitle );
	}

	ImGui::SameLine();

	ImGui::PushItemWidth( -1.f );
	const auto Ret = ImGui::SliderInt( szStrID , &Value , Min , Max );
	ImGui::PopItemWidth();

	return Ret;
}

auto CAndromedaMenu::RenderSliderFloat( const char* szTitle , const char* szStrID , float& Value , float Min , float Max ) -> bool
{
	if ( szTitle )
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text( szTitle );
	}

	ImGui::SameLine();

	ImGui::PushItemWidth( -1.f );
	const auto Ret = ImGui::SliderFloat( szStrID , &Value , Min , Max );
	ImGui::PopItemWidth();

	return Ret;
}

auto GetAndromedaMenu() -> CAndromedaMenu*
{
	return &g_CAndromedaMenu;
}
