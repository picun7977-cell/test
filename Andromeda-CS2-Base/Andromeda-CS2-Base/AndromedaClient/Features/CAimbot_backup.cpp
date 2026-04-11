#include "CAimbot.hpp"
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <GameClient/CEntityCache/CEntityCache.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bypass.hpp>
#include <AndromedaClient/Settings/Settings.hpp>

CAimbot g_Aimbot;

static Vector3 GetEntityOrigin(CCSPlayerController* pController)
{
    if (!pController) return Vector3(0.f, 0.f, 0.f);
    auto* pNode = pController->m_pGameSceneNode();
    if (!pNode) return Vector3(0.f, 0.f, 0.f);
    return pNode->m_vecAbsOrigin();
}

void CAimbot::OnCreateMove(CCSGOInput* pInput, CUserCmd* pCmd, CCSPlayerController* pLocalPlayer)
{
    if (!Settings::Aimbot::bEnabled)
        return;

    if (!pInput || !pCmd || !pLocalPlayer)
        return;

    // Получаем реальные текущие углы
    QAngle curAngles;
    curAngles.m_x = pCmd->cmd.base().viewangles().x();
    curAngles.m_y = pCmd->cmd.base().viewangles().y();
    curAngles.m_z = 0.f;

    CCSPlayerController* pBestTarget = GetBestTarget(pLocalPlayer, curAngles);
    if (!pBestTarget)
        return;

    Vector3 vTargetPos = GetTargetPosition(pBestTarget);
    Vector3 vLocalPos = GetEntityOrigin(pLocalPlayer) + Vector3(0.f, 0.f, 64.f);

    Vector3 d = vTargetPos - vLocalPos;
    float dist2D = sqrtf(d.m_x * d.m_x + d.m_y * d.m_y);
    QAngle angAimAngle(
        -atan2f(d.m_z, dist2D) * (180.f / 3.14159265f),
         atan2f(d.m_y, d.m_x)  * (180.f / 3.14159265f),
        0.f
    );

    if (Settings::Aimbot::bSmoothing)
        angAimAngle = SmoothAngles(curAngles, angAimAngle, Settings::Aimbot::fSmoothFactor);

    // Используем CL_Bypass для правильной установки углов
    GetCL_Bypass()->SetViewAngles(&angAimAngle, pInput, pCmd, true, false);
}

CCSPlayerController* CAimbot::GetBestTarget(CCSPlayerController* pLocalPlayer, const QAngle& curAngles)
{
    if (!pLocalPlayer)
        return nullptr;

    CCSPlayerController* pBestTarget = nullptr;
    float fBestFOV = Settings::Aimbot::fFOV;

    // Eye position
    Vector3 vLocalPos = GetEntityOrigin(pLocalPlayer) + Vector3(0.f, 0.f, 64.f);

    const auto& CachedVec = GetEntityCache()->GetCachedEntity();
    if (!CachedVec)
        return nullptr;

    for (const auto& CachedEntity : *CachedVec)
    {
        if (CachedEntity.m_Type != CachedEntity_t::PLAYER_CONTROLLER)
            continue;

        auto pEntity = CachedEntity.m_Handle.Get();
        if (!pEntity)
            continue;

        auto* pPlayer = reinterpret_cast<CCSPlayerController*>(pEntity);

        if (!IsValidTarget(pPlayer, pLocalPlayer))
            continue;

        Vector3 vTargetPos = GetTargetPosition(pPlayer);
        float fFOV = GetFOV(curAngles, vLocalPos, vTargetPos);

        if (fFOV < fBestFOV)
        {
            fBestFOV    = fFOV;
            pBestTarget = pPlayer;
        }
    }

    return pBestTarget;
}

bool CAimbot::IsValidTarget(CCSPlayerController* pTarget, CCSPlayerController* pLocalPlayer)
{
    if (!pTarget || pTarget == pLocalPlayer)
        return false;

    if (!pTarget->m_bPawnIsAlive())
        return false;

    if (Settings::Aimbot::bTeamCheck)
    {
        if (pTarget->m_iTeamNum() == pLocalPlayer->m_iTeamNum())
            return false;
    }

    return true;
}

Vector3 CAimbot::GetTargetPosition(CCSPlayerController* pTarget)
{
    Vector3 vOrigin = GetEntityOrigin(pTarget);

    switch (Settings::Aimbot::iAimBone)
    {
        case 0: return vOrigin + Vector3(0.f, 0.f, 68.f); // голова
        case 1: return vOrigin + Vector3(0.f, 0.f, 50.f); // грудь
        case 2: return vOrigin + Vector3(0.f, 0.f, 30.f); // таз
        default: return vOrigin + Vector3(0.f, 0.f, 68.f);
    }
}

QAngle CAimbot::SmoothAngles(const QAngle& current, const QAngle& target, float smooth)
{
    QAngle delta = target - current;

    while (delta.m_x > 180.f)  delta.m_x -= 360.f;
    while (delta.m_x < -180.f) delta.m_x += 360.f;
    while (delta.m_y > 180.f)  delta.m_y -= 360.f;
    while (delta.m_y < -180.f) delta.m_y += 360.f;

    if (delta.m_x >  smooth) delta.m_x =  smooth;
    if (delta.m_x < -smooth) delta.m_x = -smooth;
    if (delta.m_y >  smooth) delta.m_y =  smooth;
    if (delta.m_y < -smooth) delta.m_y = -smooth;

    return current + delta;
}

float CAimbot::GetFOV(const QAngle& viewAngles, const Vector3& src, const Vector3& dst)
{
    Vector3 vDelta = dst - src;
    float flDist = vDelta.Length();
    if (flDist <= 0.f) return 0.f;

    float flPitch = asinf(vDelta.m_z / flDist) * (180.f / 3.14159265f);
    float flYaw   = atan2f(vDelta.m_y, vDelta.m_x) * (180.f / 3.14159265f);

    QAngle ang(flPitch, flYaw, 0.f);
    QAngle delta = ang - viewAngles;

    while (delta.m_x > 180.f)  delta.m_x -= 360.f;
    while (delta.m_x < -180.f) delta.m_x += 360.f;
    while (delta.m_y > 180.f)  delta.m_y -= 360.f;
    while (delta.m_y < -180.f) delta.m_y += 360.f;

    return sqrtf(delta.m_x * delta.m_x + delta.m_y * delta.m_y);
}
