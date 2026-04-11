#include "CRageBot.hpp"
#include <CS2/SDK/SDK.hpp>
#include <CS2/SDK/Math/Math.hpp>
#include <CS2/SDK/Update/CCSGOInput.hpp>
#include <GameClient/CEntityCache/CEntityCache.hpp>
#include <GameClient/CL_Players.hpp>
#include <GameClient/CL_Bypass.hpp>
#include <AndromedaClient/Settings/Settings.hpp>

CRageBot g_RageBot;

static Vector3 GetEntityOrigin(CCSPlayerController* p)
{
    if (!p) return {};
    auto* n = p->m_pGameSceneNode();
    if (!n) return {};
    return n->m_vecAbsOrigin();
}

static QAngle GetViewAngles(CUserCmd* pCmd)
{
    return QAngle(
        pCmd->cmd.base().viewangles().x(),
        pCmd->cmd.base().viewangles().y(),
        0.f
    );
}

void CRageBot::OnCreateMove(CCSGOInput* pInput, CUserCmd* pCmd, CCSPlayerController* pLocalPlayer)
{
    if (!Settings::RageBot::bEnabled || !pInput || !pCmd || !pLocalPlayer)
        return;

    if (!GetEntityCache())
        return;

    m_RealAngles = GetViewAngles(pCmd);

    if (Settings::RageBot::bAutoScope)
        ApplyAutoScope(pCmd, pLocalPlayer);

    CCSPlayerController* pTarget = GetBestTarget(pCmd, pLocalPlayer);
    m_pCurrentTarget = pTarget;

    if (!pTarget)
    {
        m_bShooting = false;
        if (Settings::RageBot::bAntiAim)
            ApplyAntiAim(pInput, pCmd);
        return;
    }

    if (Settings::RageBot::bBacktrack)
        UpdateBacktrack(pTarget);

    Vector3 vAimPos   = GetBestHitbox(pTarget);
    Vector3 vLocalEye = GetEntityOrigin(pLocalPlayer) + Vector3(0.f, 0.f, 64.f);
    QAngle  angAim    = CalcAngle(vLocalEye, vAimPos);
    NormalizeAngle(angAim);

    if (Settings::RageBot::bSilentAim)
    {
        GetCL_Bypass()->SetViewAngles(&angAim, pInput, pCmd, true, false);
        if (ShouldShoot(pCmd, pLocalPlayer, pTarget))
        {
            GetCL_Bypass()->SetAttack(pCmd, true);
            m_bShooting = true;
        }
        QAngle restore = m_RealAngles;
        GetCL_Bypass()->SetViewAngles(&restore, pInput, pCmd, true, false);
    }
    else
    {
        GetCL_Bypass()->SetViewAngles(&angAim, pInput, pCmd, true, false);
        if (ShouldShoot(pCmd, pLocalPlayer, pTarget))
        {
            GetCL_Bypass()->SetAttack(pCmd, true);
            m_bShooting = true;
        }
    }

    if (Settings::RageBot::bAutoStop)
        ApplyAutoStop(pCmd);

    if (Settings::RageBot::bAntiAim)
        ApplyAntiAim(pInput, pCmd);
}

void CRageBot::OnPostCreateMove(CUserCmd* pCmd)
{
    m_bShooting = false;
}

CCSPlayerController* CRageBot::GetBestTarget(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer)
{
    if (!pCmd || !pLocalPlayer) return nullptr;

    CCSPlayerController* pBest = nullptr;
    float fBestFOV = Settings::RageBot::fFOV;

    Vector3 vLocalEye = GetEntityOrigin(pLocalPlayer) + Vector3(0.f, 0.f, 64.f);
    QAngle  curAngles = m_RealAngles;

    const auto& CachedVec = GetEntityCache()->GetCachedEntity();
    if (!CachedVec) return nullptr;

    for (const auto& e : *CachedVec)
    {
        if (e.m_Type != CachedEntity_t::PLAYER_CONTROLLER) continue;
        auto* pEnt = e.m_Handle.Get();
        if (!pEnt) continue;

        auto* pPlayer = reinterpret_cast<CCSPlayerController*>(pEnt);
        if (!IsValidTarget(pPlayer, pLocalPlayer)) continue;

        float fFOV = GetFOV(curAngles, vLocalEye, GetBestHitbox(pPlayer));
        if (fFOV < fBestFOV)
        {
            fBestFOV = fFOV;
            pBest    = pPlayer;
        }
    }

    return pBest;
}

bool CRageBot::IsValidTarget(CCSPlayerController* pTarget, CCSPlayerController* pLocalPlayer)
{
    if (!pTarget || pTarget == pLocalPlayer) return false;
    if (!pTarget->m_bPawnIsAlive()) return false;
    if (Settings::RageBot::bTeamCheck && pTarget->m_iTeamNum() == pLocalPlayer->m_iTeamNum()) return false;
    return true;
}

float CRageBot::CalculateHitchance(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer, CCSPlayerController* pTarget)
{
    if (!pCmd || !pLocalPlayer || !pTarget) return 0.f;

    Vector3 vLocalEye  = GetEntityOrigin(pLocalPlayer) + Vector3(0.f, 0.f, 64.f);
    Vector3 vTargetPos = GetBestHitbox(pTarget);
    QAngle curAngles   = GetViewAngles(pCmd);

    float fFOV = GetFOV(curAngles, vLocalEye, vTargetPos);
    float fHitchance = 100.f - (fFOV / 5.f * 100.f);
    if (fHitchance < 0.f)   fHitchance = 0.f;
    if (fHitchance > 100.f) fHitchance = 100.f;
    return fHitchance;
}

bool CRageBot::ShouldShoot(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer, CCSPlayerController* pTarget)
{
    if (!Settings::RageBot::bAutoShoot) return false;
    if (!Settings::RageBot::bHitchance) return true;
    return CalculateHitchance(pCmd, pLocalPlayer, pTarget) >= Settings::RageBot::fMinHitchance;
}

void CRageBot::ApplyAntiAim(CCSGOInput* pInput, CUserCmd* pCmd)
{
    QAngle ang = m_RealAngles;

    switch (Settings::RageBot::iAntiAimType)
    {
        case 1:
        {
            static bool bFlip = false;
            bFlip = !bFlip;
            ang.m_y += bFlip ? 90.f : -90.f;
            break;
        }
        case 2:
        {
            static float fSpin = 0.f;
            fSpin += 25.f;
            if (fSpin > 180.f) fSpin -= 360.f;
            ang.m_y += fSpin;
            break;
        }
        default:
            return;
    }

    NormalizeAngle(ang);
    GetCL_Bypass()->SetViewAngles(&ang, pInput, pCmd, true, false);
}

Vector3 CRageBot::GetBestHitbox(CCSPlayerController* pTarget)
{
    Vector3 v = GetEntityOrigin(pTarget);
    switch (Settings::RageBot::iAimBone)
    {
        case 0: return v + Vector3(0.f, 0.f, 68.f);
        case 1: return v + Vector3(0.f, 0.f, 50.f);
        case 2: return v + Vector3(0.f, 0.f, 30.f);
        default: return v + Vector3(0.f, 0.f, 68.f);
    }
}

void CRageBot::ApplyAutoStop(CUserCmd* pCmd)
{
    pCmd->cmd.mutable_base()->set_forwardmove(0.f);
    pCmd->cmd.mutable_base()->set_leftmove(0.f);
    pCmd->cmd.mutable_base()->set_upmove(0.f);
}

void CRageBot::ApplyAutoScope(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer)
{
    if (!pCmd || !pLocalPlayer) return;
    auto h = pLocalPlayer->m_hPawn();
    if (!h.IsValid()) return;
    auto* pPawn = h.Get();
    if (!pPawn) return;
    auto* pCSPawn = reinterpret_cast<C_CSPlayerPawn*>(pPawn);
    if (pCSPawn && !pCSPawn->m_bIsScoped())
    {
        pCmd->button_states.buttonstate1 |= IN_ATTACK2;
        pCmd->cmd.mutable_base()->mutable_buttons_pb()->set_buttonstate1(pCmd->button_states.buttonstate1);
    }
}

void CRageBot::UpdateBacktrack(CCSPlayerController* pPlayer)
{
    if (!pPlayer) return;
    auto* pId = pPlayer->pEntityIdentity();
    if (!pId) return;
    int idx = pId->Handle().GetEntryIndex();
    if (idx < 0 || idx >= 64) return;

    int head = m_iBacktrackHead[idx];
    m_BacktrackRecords[idx][head].vOrigin   = GetEntityOrigin(pPlayer);
    m_BacktrackRecords[idx][head].flSimTime = 0.f;
    m_iBacktrackHead[idx] = (head + 1) % MAX_BACKTRACK_RECORDS;
}

bool CRageBot::ApplyBacktrack(CUserCmd* pCmd, CCSPlayerController* pTarget)
{
    return false; // требует симтайм
}

float CRageBot::GetFOV(const QAngle& va, const Vector3& src, const Vector3& dst)
{
    Vector3 d = dst - src;
    float dist = d.Length();
    if (dist <= 0.f) return 0.f;

    QAngle ang(
        asinf(d.m_z / dist) * (180.f / 3.14159265f),
        atan2f(d.m_y, d.m_x) * (180.f / 3.14159265f),
        0.f
    );
    QAngle delta = ang - va;
    NormalizeAngle(delta);
    return sqrtf(delta.m_x * delta.m_x + delta.m_y * delta.m_y);
}

QAngle CRageBot::CalcAngle(const Vector3& src, const Vector3& dst)
{
    Vector3 d = dst - src;
    float dist2D = sqrtf(d.m_x * d.m_x + d.m_y * d.m_y);
    return QAngle(
        -atan2f(d.m_z, dist2D) * (180.f / 3.14159265f),
        atan2f(d.m_y, d.m_x)   * (180.f / 3.14159265f),
        0.f
    );
}

void CRageBot::NormalizeAngle(QAngle& a)
{
    while (a.m_x >  89.f)  a.m_x -= 180.f;
    while (a.m_x < -89.f)  a.m_x += 180.f;
    while (a.m_y >  180.f) a.m_y -= 360.f;
    while (a.m_y < -180.f) a.m_y += 360.f;
    a.m_z = 0.f;
}
// ============================================================
// Additional Ragebot Features
// ============================================================

Vector3 CRageBot::GetEyePosition(CCSPlayerController* p)
{
    if (!p) return {};
    auto* n = p->m_pGameSceneNode();
    if (!n) return {};
    Vector3 origin = n->m_vecAbsOrigin();
    // Get view offset - may need to be adjusted based on SDK
    return origin + Vector3(0.f, 0.f, 64.f); // approximate eye height
}

Vector3 CRageBot::GetBonePosition(CCSPlayerController* p, int boneIndex)
{
    if (!p) return {};
    auto* n = p->m_pGameSceneNode();
    if (!n) return {};
    
    // Get model state and bone matrix
    // This is a simplified version - actual implementation may vary
    auto* modelState = n->m_modelState();
    if (!modelState) return {};
    
    // Bone matrix is typically at modelState + 0x80
    // Each bone is 32 bytes (4x4 matrix)
    // Return head bone position (bone 6 in CS2)
    return GetEntityOrigin(p) + Vector3(0.f, 0.f, 68.f); // fallback
}

Vector3 CRageBot::ScanMultipoint(CCSPlayerController* pLocalPlayer, CCSPlayerController* pTargetPawn, const Vector3& headCenter)
{
    const float radius = 4.0f; // head hitbox radius
    const int pointsPerAxis = 3; // 3x3x3 = 27 points
    float step = (radius * 2.0f) / (pointsPerAxis - 1);
    
    Vector3 bestPoint = headCenter;
    float bestDamage = 0.0f;
    
    for (int ix = 0; ix < pointsPerAxis; ix++)
    {
        for (int iy = 0; iy < pointsPerAxis; iy++)
        {
            for (int iz = 0; iz < pointsPerAxis; iz++)
            {
                Vector3 offset;
                offset.m_x = -radius + ix * step;
                offset.m_y = -radius + iy * step;
                offset.m_z = -radius + iz * step;
                
                Vector3 point = headCenter + offset;
                
                // Check visibility (simplified - needs proper trace implementation)
                if (true) // Replace with actual trace check
                {
                    return point;
                }
            }
        }
    }
    return headCenter; // fallback
}

void CRageBot::ResolveTarget(CCSPlayerController* pTarget)
{
    if (!pTarget) return;
    
    // Basic resolver - read lower body yaw and eye angles
    // This is a placeholder - implement proper resolver logic
}

bool CRageBot::CanShoot(CCSPlayerController* pLocalPlayer)
{
    if (!pLocalPlayer) return false;
    
    auto h = pLocalPlayer->m_hPawn();
    if (!h.IsValid()) return false;
    
    auto* pPawn = h.Get();
    if (!pPawn) return false;
    
    auto* pCSPawn = reinterpret_cast<C_CSPlayerPawn*>(pPawn);
    if (!pCSPawn) return false;
    
    // Check weapon clip and next attack time
    // This requires accessing weapon services - implementation depends on SDK
    return true;
}
