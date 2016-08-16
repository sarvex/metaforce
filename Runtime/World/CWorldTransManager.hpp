#ifndef __URDE_CWORLDTRANSMANAGER_HPP__
#define __URDE_CWORLDTRANSMANAGER_HPP__

#include "RetroTypes.hpp"
#include "CRandom16.hpp"
#include "Character/CModelData.hpp"
#include "GuiSys/CGuiTextSupport.hpp"
#include "Graphics/CLight.hpp"

namespace urde
{
class CSimplePool;
class CStringTable;

class CWorldTransManager
{
public:
    enum class ETransType
    {
        Disabled,
        Enabled,
        Text
    };

    struct SModelDatas
    {
        CAnimRes x0_samusRes;
        CModelData x1c_samusModelData;
        CModelData x68_beamModelData;
        CModelData xb4_platformModelData;
        CModelData x100_bgModelData;
        TLockedToken<CModel> x14c_beamModel;
        TLockedToken<CModel> x158_suitModel;
        TLockedToken<CSkinRules> x164_suitSkin;
        zeus::CTransform x170_gunXf;
        std::vector<CLight> x1a0_lights;
        //std::unique_ptr<u8> x1b0_dissolveTextureBuffer;
        zeus::CVector2f x1b4_shakeResult;
        zeus::CVector2f x1bc_shakeDelta;
        float x1c4_randTimeout = 0.f;
        float x1c8_blurResult = 0.f;
        float x1cc_blurDelta = 0.f;
        float x1d0_dissolveStartTime = 99999.f;
        float x1d4_relativeDissolveStartTime = 99999.f;
        float x1d8_relativeDissolveEndTime = 99999.f;
        bool x1dc_dissolveStarted = false;

        SModelDatas(const CAnimRes& samusRes);
    };

private:
    float x0_curTime = 0.f;
    std::unique_ptr<SModelDatas> x4_modelData;
    std::unique_ptr<CGuiTextSupport> x8_textData;
    TLockedToken<CStringTable> xc_strTable;
    u8 x14_ = 0;
    float x18_bgOffset;
    float x1c_bgHeight;
    CRandom16 x20_random = CRandom16(99);
    u16 x24_ = 1189;
    u32 x28_ = 0;
    u8 x2c_ = 127;
    u8 x2d_ = 64;
    ETransType x30_type = ETransType::Disabled;
    float x38_ = 0.f;
    bool x40_;
    union
    {
        struct
        {
            bool x44_24_dissolveComplete : 1;
            bool x44_25_stopSoon : 1;
            bool x44_26_goingUp : 1;
            bool x44_27_ : 1;
            bool x44_28_ : 1;
        };
        u8 dummy = 0;
    };

    static int GetSuitCharSet();

public:
    CWorldTransManager() : x44_24_dissolveComplete(true) {}

    void DrawFirstPass() const {}
    void DrawSecondPass() const {}
    void DrawAllModels() const {}
    void UpdateLights(float dt);
    void UpdateEnabled(float);
    void UpdateDisabled(float);
    void UpdateText(float);
    void Update(float);
    void DrawEnabled() const;
    void DrawDisabled() const;
    void DrawText() const;
    void Draw() const;

    void EnableTransition(const CAnimRes& samusRes,
                          ResId platRes, const zeus::CVector3f& platScale,
                          ResId bgRes, const zeus::CVector3f& bgScale, bool goingUp);
    void EnableTransition(ResId fontId, ResId stringId, bool b1, bool b2,
                          float chFadeTime, float chFadeRate, float f3);

    void StartTransition();
    void EndTransition();
    void PleaseStopSoon() { x44_25_stopSoon = true; }
    bool IsTransitionEnabled() const { return x30_type != ETransType::Disabled; }
    void DisableTransition();
    void TouchModels();
};

}

#endif // __URDE_CWORLDTRANSMANAGER_HPP__
