#pragma once

#include <Common/Common.hpp>
#include <CS2/SDK/Math/Vector3.hpp>
#include <CS2/SDK/Math/QAngle.hpp>
#include <CS2/SDK/Update/CUserCmd.hpp>

class CCSPlayerController;
class CCSGOInput;
class C_CSPlayerPawn;

struct BacktrackRecord_t
{
    Vector3 vOrigin;
    float   flSimTime;
};

// ============================================================
// Ragebot for Counter-Strike 2 (Internal DLL)
// Target: CS2 (Source 2 engine)
// Features: Aimbot (Rage), Auto Fire, Multipoint, Basic Resolver
// ============================================================

class CRageBot
{
public:
    CRageBot() = default;
    ~CRageBot() = default;

    void OnCreateMove(CCSGOInput* pInput, CUserCmd* pCmd, CCSPlayerController* pLocalPlayer);
    void OnPostCreateMove(CUserCmd* pCmd);
    void UpdateBacktrack(CCSPlayerController* pPlayer);

private:
    // Target selection
    CCSPlayerController* GetBestTarget(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer);
    bool IsValidTarget(CCSPlayerController* pTarget, CCSPlayerController* pLocalPlayer);
    
    // Hitchance - вероятность попадания
    float CalculateHitchance(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer, CCSPlayerController* pTarget);
    bool ShouldShoot(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer, CCSPlayerController* pTarget);
    
    // Silent Aim - наведение без изменения view angles
    void ApplySilentAim(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer, CCSPlayerController* pTarget);
    
    // Anti-Aim - развороты для защиты
    void ApplyAntiAim(CCSGOInput* pInput, CUserCmd* pCmd);
    
    // Thirdperson - камера от третьего лица
    void ApplyThirdperson(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer);
    
    // Utilities
    Vector3 GetBestHitbox(CCSPlayerController* pTarget);
    void ApplyAutoStop(CUserCmd* pCmd);
    void ApplyAutoScope(CUserCmd* pCmd, CCSPlayerController* pLocalPlayer);
    bool ApplyBacktrack(CUserCmd* pCmd, CCSPlayerController* pTarget);
    float GetFOV(const QAngle& viewAngles, const Vector3& src, const Vector3& dst);
    QAngle CalcAngle(const Vector3& src, const Vector3& dst);
    void NormalizeAngle(QAngle& angle);
    
    // Additional ragebot features
    Vector3 ScanMultipoint(CCSPlayerController* pLocalPlayer, CCSPlayerController* pTargetPawn, const Vector3& headCenter);
    void ResolveTarget(CCSPlayerController* pTarget);
    bool CanShoot(CCSPlayerController* pLocalPlayer);
    
    // Get entity origin helper
    Vector3 GetEntityOrigin(CCSPlayerController* p);
    Vector3 GetEyePosition(CCSPlayerController* p);
    Vector3 GetBonePosition(CCSPlayerController* p, int boneIndex);

private:
    static constexpr int MAX_BACKTRACK_RECORDS = 16;

    CCSPlayerController* m_pCurrentTarget = nullptr;
    bool                 m_bShooting      = false;
    QAngle               m_LastAntiAimAngle;
    QAngle               m_RealAngles;     // реальные углы игрока до изменений
    int                  m_iAntiAimType   = 0;

    BacktrackRecord_t m_BacktrackRecords[64][MAX_BACKTRACK_RECORDS];
    int               m_iBacktrackHead[64] = {};
};

extern CRageBot g_RageBot;
