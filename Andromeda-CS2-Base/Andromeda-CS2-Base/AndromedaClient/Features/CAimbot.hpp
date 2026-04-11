#pragma once

#include <Common/Common.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

class CCSPlayerController;
class CCSGOInput;
class C_CSPlayerPawn;

class CAimbot
{
public:
    CAimbot() = default;
    ~CAimbot() = default;

    void OnCreateMove(CCSGOInput* pInput, CUserCmd* pCmd, CCSPlayerController* pLocalPlayer);
    CCSPlayerController* GetBestTarget(CCSPlayerController* pLocalPlayer, const QAngle& curAngles);
    bool IsValidTarget(CCSPlayerController* pTarget, CCSPlayerController* pLocalPlayer);
    Vector3 GetTargetPosition(CCSPlayerController* pTarget);
    float GetFOV(const QAngle& viewAngles, const Vector3& src, const Vector3& dst);
    QAngle SmoothAngles(const QAngle& current, const QAngle& target, float smoothFactor);

private:
    CCSPlayerController* m_pCurrentTarget = nullptr;
    Vector3 m_vLastTargetPos;
    float m_flLastUpdateTime = 0.0f;
    QAngle m_LastAngles;
    bool m_bTargetLocked = false;
    int m_iCurrentTargetID = -1;
};

extern CAimbot g_Aimbot;
