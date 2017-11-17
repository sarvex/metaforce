#include "CPauseScreenBlur.hpp"
#include "CSimplePool.hpp"
#include "GameGlobalObjects.hpp"
#include "Audio/CSfxManager.hpp"

namespace urde
{
namespace MP1
{

CPauseScreenBlur::CPauseScreenBlur()
: x4_mapLightQuarter(g_SimplePool->GetObj("TXTR_MapLightQuarter"))
{
    x50_25_gameDraw = true;
}

void CPauseScreenBlur::OnNewInGameGuiState(EInGameGuiState state, CStateManager& stateMgr)
{
    switch (state)
    {
    case EInGameGuiState::Zero:
    case EInGameGuiState::InGame:
        SetState(EState::InGame);
        break;
    case EInGameGuiState::MapScreen:
        SetState(EState::MapScreen);
        break;
    case EInGameGuiState::PauseSaveGame:
        SetState(EState::SaveGame);
        break;
    case EInGameGuiState::PauseHUDMessage:
        SetState(EState::HUDMessage);
        break;
    case EInGameGuiState::PauseGame:
    case EInGameGuiState::PauseLogBook:
        SetState(EState::Pause);
        break;
    default: break;
    }
}

void CPauseScreenBlur::SetState(EState state)
{
    if (x10_prevState == EState::InGame && state != EState::InGame)
    {
        CSfxManager::SetChannel(CSfxManager::ESfxChannels::PauseScreen);
        if (state == EState::HUDMessage)
            CSfxManager::SfxStart(1415, 1.f, 0.f, false, 0x7f, false, kInvalidAreaId);
        else if (state == EState::MapScreen)
            CSfxManager::SfxStart(1378, 1.f, 0.f, false, 0x7f, false, kInvalidAreaId);
        x18_blurAmt = FLT_EPSILON;
    }

    if (state == EState::InGame && (x10_prevState != EState::InGame || x14_nextState != EState::InGame))
    {
        CSfxManager::SetChannel(CSfxManager::ESfxChannels::Game);
        if (x10_prevState == EState::HUDMessage)
            CSfxManager::SfxStart(1416, 1.f, 0.f, false, 0x7f, false, kInvalidAreaId);
        else if (x10_prevState == EState::MapScreen)
            CSfxManager::SfxStart(1380, 1.f, 0.f, false, 0x7f, false, kInvalidAreaId);
        x18_blurAmt = -1.f;
    }

    x14_nextState = state;
}

void CPauseScreenBlur::OnBlurComplete(bool b)
{
    if (x14_nextState == EState::InGame && !b)
        return;
    x10_prevState = x14_nextState;
    if (x10_prevState == EState::InGame)
        x50_25_gameDraw = true;
}

void CPauseScreenBlur::Update(float dt, const CStateManager& stateMgr, bool b)
{
    if (x10_prevState == x14_nextState)
        return;

    if (x18_blurAmt < 0.f)
        x18_blurAmt = std::min(0.f, 2.f * dt + x18_blurAmt);
    else
        x18_blurAmt = std::min(1.f, 2.f * dt + x18_blurAmt);

    if (x18_blurAmt == 0.f || x18_blurAmt == 1.f)
        OnBlurComplete(b);

    if (x18_blurAmt == 0.f && b)
    {
        x1c_camBlur.DisableBlur(0.f);
    }
    else
    {
        x1c_camBlur.SetBlur(EBlurType::HiBlur,
                            g_tweakGui->GetPauseBlurFactor() * std::fabs(x18_blurAmt), 0.f);
        x50_24_blurring = true;
    }
}

void CPauseScreenBlur::Draw(const CStateManager&) const
{
    const_cast<CCameraBlurPass&>(x1c_camBlur).Draw();
    float t = std::fabs(x18_blurAmt);
    if (x1c_camBlur.GetCurrType() != EBlurType::NoBlur)
    {
        zeus::CColor filterColor =
            zeus::CColor::lerp(zeus::CColor::skWhite, g_tweakGuiColors->GetPauseBlurFilterColor(), t);
        const_cast<CTexturedQuadFilter&>(m_quarterFilter).DrawFilter(EFilterShape::FullscreenQuarters,
                                                                     filterColor, t);
        zeus::CColor scanLinesColor =
            zeus::CColor::lerp(zeus::CColor::skWhite, zeus::CColor(0.75f, 1.f), t);
        //const_cast<CScanLinesFilterEven&>(m_linesFilter).draw(scanLinesColor);
    }

    if (x50_24_blurring /*&& x1c_camBlur.x2d_noPersistentCopy*/)
    {
        const_cast<CPauseScreenBlur*>(this)->x50_24_blurring = false;
        const_cast<CPauseScreenBlur*>(this)->x50_25_gameDraw = false;
    }
}

}
}
