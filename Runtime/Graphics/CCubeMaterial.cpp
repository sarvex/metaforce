#include "Graphics/CCubeMaterial.hpp"

#include "GameGlobalObjects.hpp"
#include "Graphics/CCubeModel.hpp"
#include "Graphics/CCubeRenderer.hpp"
#include "Graphics/CCubeSurface.hpp"
#include "Graphics/CModel.hpp"

#include <aurora/model.hpp>

namespace metaforce {
static u32 sReflectionType = 0;
static u32 sLastMaterialUnique = UINT32_MAX;
static const u8* sLastMaterialCached = nullptr;
static const CCubeModel* sLastModelCached = nullptr;
static const CCubeModel* sRenderingModel = nullptr;
static float sReflectionAlpha = 0.f;

void CCubeMaterial::SetCurrent(const CModelFlags& flags, const CCubeSurface& surface, CCubeModel& model) {
  if (sLastMaterialCached == x0_data) {
    if (sReflectionType == 1) {
      if (sLastModelCached == sRenderingModel) {
        return;
      }
    } else if (sReflectionType != 2) {
      return;
    }
  }

  if (CCubeModel::sRenderModelBlack) {
    SetCurrentBlack();
    return;
  }

  sRenderingModel = &model;
  sLastMaterialCached = x0_data;

  u32 numIndStages = 0;
  const auto matFlags = GetFlags();
  const u8* materialDataCur = x0_data;

  const bool reflection = bool(
      matFlags & (Flags(CCubeMaterialFlagBits::fSamusReflection) | CCubeMaterialFlagBits::fSamusReflectionSurfaceEye));
  if (reflection) {
    if (!(matFlags & CCubeMaterialFlagBits::fSamusReflectionSurfaceEye)) {
      EnsureViewDepStateCached(nullptr);
    } else {
      EnsureViewDepStateCached(&surface);
    }
  }

  u32 texCount = SBig(*reinterpret_cast<const u32*>(materialDataCur + 4));
  if (flags.x2_flags & CModelFlagBits::NoTextureLock) {
    materialDataCur += (2 + texCount) * 4;
  } else {
    materialDataCur += 8;
    for (u32 i = 0; i < texCount; ++i) {
      u32 texIdx = SBig(*reinterpret_cast<const u32*>(materialDataCur));
      sRenderingModel->GetTexture(texIdx)->Load(static_cast<GX::TexMapID>(i), EClampMode::Repeat);
      materialDataCur += 4;
    }
  }

  auto groupIdx = SBig(*reinterpret_cast<const u32*>(materialDataCur + 4));
  if (sLastMaterialUnique != UINT32_MAX && sLastMaterialUnique == groupIdx && sReflectionType == 0) {
    return;
  }
  sLastMaterialUnique = groupIdx;

  u32 vatFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur));
  aurora::gfx::model::set_vtx_desc_compressed(vatFlags);
  materialDataCur += 8;

  bool packedLightMaps = matFlags.IsSet(CCubeMaterialFlagBits::fLightmapUvArray);
  if (packedLightMaps != CCubeModel::sUsingPackedLightmaps) {
    model.SetUsingPackedLightmaps(packedLightMaps);
  }

  u32 finalKColorCount = 0;
  if (matFlags & CCubeMaterialFlagBits::fKonstValues) {
    u32 konstCount = SBig(*reinterpret_cast<const u32*>(materialDataCur));
    finalKColorCount = konstCount;
    materialDataCur += 4;
    for (u32 i = 0; i < konstCount; ++i) {
      u32 kColor = SBig(*reinterpret_cast<const u32*>(materialDataCur));
      materialDataCur += 4;
      aurora::gfx::set_tev_k_color(static_cast<GX::TevKColorID>(i), kColor);
    }
  }

  u32 blendFactors = SBig(*reinterpret_cast<const u32*>(materialDataCur));
  materialDataCur += 4;
  if (g_Renderer->IsInAreaDraw()) {
    // TODO blackout fog, additive blend
  } else {
    SetupBlendMode(blendFactors, flags, matFlags.IsSet(CCubeMaterialFlagBits::fAlphaTest));
  }

  bool indTex = matFlags.IsSet(CCubeMaterialFlagBits::fSamusReflectionIndirectTexture);
  u32 indTexSlot = 0;
  if (indTex) {
    indTexSlot = SBig(*reinterpret_cast<const u32*>(materialDataCur));
    materialDataCur += 4;
  }

  HandleDepth(flags.x2_flags, matFlags);

  u32 chanCount = SBig(*reinterpret_cast<const u32*>(materialDataCur));
  materialDataCur += 4;
  u32 firstChan = SBig(*reinterpret_cast<const u32*>(materialDataCur));
  materialDataCur += 4 * chanCount;
  u32 finalNumColorChans = HandleColorChannels(chanCount, firstChan);

  u32 firstTev = 0;
  if (CCubeModel::sRenderModelShadow)
    firstTev = 2;

  u32 matTevCount = SBig(*reinterpret_cast<const u32*>(materialDataCur));
  materialDataCur += 4;
  u32 finalTevCount = matTevCount;

  const u32* texMapTexCoordFlags = reinterpret_cast<const u32*>(materialDataCur + matTevCount * 20);
  const u32* tcgs = reinterpret_cast<const u32*>(texMapTexCoordFlags + matTevCount);
  bool usesTevReg2 = false;

  u32 finalCCFlags = 0;
  u32 finalACFlags = 0;

  if (g_Renderer->IsThermalVisorActive()) {
    finalTevCount = firstTev + 1;
    u32 ccFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur + 8));
    finalCCFlags = ccFlags;
    auto outputReg = static_cast<GX::TevRegID>(ccFlags >> 9 & 0x3);
    if (outputReg == GX::TEVREG0) {
      materialDataCur += 20;
      texMapTexCoordFlags += 1;
      finalCCFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur + 8));
      aurora::gfx::set_tev_reg_color(GX::TEVREG0, 0xc0c0c0c0);
    }
    finalACFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur + 12));
    HandleTev(firstTev, reinterpret_cast<const u32*>(materialDataCur), texMapTexCoordFlags,
              CCubeModel::sRenderModelShadow);
    usesTevReg2 = false;
  } else {
    finalTevCount = firstTev + matTevCount;
    for (u32 i = firstTev; i < finalTevCount; ++i) {
      HandleTev(i, reinterpret_cast<const u32*>(materialDataCur), texMapTexCoordFlags,
                CCubeModel::sRenderModelShadow && i == firstTev);
      u32 ccFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur + 8));
      finalCCFlags = ccFlags;
      finalACFlags = SBig(*reinterpret_cast<const u32*>(materialDataCur + 12));
      auto outputReg = static_cast<GX::TevRegID>(ccFlags >> 9 & 0x3);
      if (outputReg == GX::TEVREG2) {
        usesTevReg2 = true;
      }
      materialDataCur += 20;
      texMapTexCoordFlags += 1;
    }
  }

  u32 tcgCount = 0;
  if (g_Renderer->IsThermalVisorActive()) {
    u32 fullTcgCount = SBig(*tcgs);
    tcgCount = std::min(fullTcgCount, 2u);
    for (u32 i = 0; i < tcgCount; ++i) {
      // TODO set TCG
    }
    tcgs += fullTcgCount + 1;
  } else {
    tcgCount = SBig(*tcgs);
    for (u32 i = 0; i < tcgCount; ++i) {
      // TODO set TCG
    }
    tcgs += tcgCount + 1;
  }

  const u32* uvAnim = tcgs;
  u32 animCount = uvAnim[1];
  uvAnim += 2;
  u32 texMtx = 30;
  u32 pttTexMtx = 64;
  for (u32 i = 0; i < animCount; ++i) {
    u32 size = HandleAnimatedUV(uvAnim, texMtx, pttTexMtx);
    if (size == 0)
      break;
    uvAnim += size;
    texMtx += 3;
    pttTexMtx += 3;
  }

  if (flags.x0_blendMode != 0) {
    HandleTransparency(finalTevCount, finalKColorCount, flags, blendFactors, finalCCFlags, finalACFlags);
  }

  if (reflection) {
    if (sReflectionAlpha > 0.f) {
      u32 additionalTevs = 0;
      if (indTex) {
        additionalTevs = HandleReflection(usesTevReg2, indTexSlot, 0, finalTevCount, texCount, tcgCount,
                                          finalKColorCount, finalCCFlags, finalACFlags);
        numIndStages = 1;
        tcgCount += 2;
      } else {
        additionalTevs = HandleReflection(usesTevReg2, 255, 0, finalTevCount, texCount, tcgCount, finalKColorCount,
                                          finalCCFlags, finalACFlags);
        tcgCount += 1;
      }
      texCount += 1;
      finalTevCount += additionalTevs;
      finalKColorCount += 1;
    } else if (((finalCCFlags >> 9) & 0x3) != 0) {
      DoPassthru(finalTevCount);
      finalTevCount += 1;
    }
  }

  if (CCubeModel::sRenderModelShadow) {
    DoModelShadow(texCount, tcgCount);
    tcgCount += 1;
  }

  // SetNumIndStages(numIndStages);
  aurora::gfx::disable_tev_stage(static_cast<ERglTevStage>(finalTevCount));
  // SetNumTexGens(tcgCount);
  // SetNumColorChans(finalNumColorChans);
}

void CCubeMaterial::SetCurrentBlack() {
  auto flags = GetFlags();
  auto vatFlags = GetVatFlags();

  if (flags.IsSet(CCubeMaterialFlagBits::fDepthSorting) || flags.IsSet(CCubeMaterialFlagBits::fAlphaTest)) {
    // set fog mode 0x21
    aurora::gfx::set_blend_mode(ERglBlendMode::Blend, ERglBlendFactor::Zero, ERglBlendFactor::One, ERglLogicOp::Clear);
  } else {
    // set fog mode 5
    aurora::gfx::set_blend_mode(ERglBlendMode::Blend, ERglBlendFactor::One, ERglBlendFactor::Zero, ERglLogicOp::Clear);
  }
  // set vtx desc flags
  // TODO
}

void CCubeMaterial::SetupBlendMode(u32 blendFactors, const CModelFlags& flags, bool alphaTest) {
  auto newSrcFactor = static_cast<ERglBlendFactor>(blendFactors & 0xffff);
  auto newDstFactor = static_cast<ERglBlendFactor>(blendFactors >> 16 & 0xffff);
  if (alphaTest) {
    // discard fragments with alpha < 0.25
    aurora::gfx::set_alpha_discard(true);
    newSrcFactor = ERglBlendFactor::One;
    newDstFactor = ERglBlendFactor::Zero;
  } else {
    aurora::gfx::set_alpha_discard(false);
  }

  if (flags.x0_blendMode > 4 && newSrcFactor == ERglBlendFactor::One) {
    newSrcFactor = ERglBlendFactor::SrcAlpha;
    if (newDstFactor == ERglBlendFactor::Zero) {
      newDstFactor = flags.x0_blendMode > 6 ? ERglBlendFactor::One : ERglBlendFactor::InvSrcAlpha;
    }
  }

  // TODO set fog color zero if dst blend zero
  aurora::gfx::set_blend_mode(ERglBlendMode::Blend, newSrcFactor, newDstFactor, ERglLogicOp::Clear);
}

void CCubeMaterial::HandleDepth(CModelFlagsFlags modelFlags, CCubeMaterialFlags matFlags) {
  ERglEnum func = ERglEnum::Never;
  if (!(modelFlags & CModelFlagBits::DepthTest)) {
    func = ERglEnum::Always;
  } else if (modelFlags & CModelFlagBits::DepthGreater) {
    func = modelFlags & CModelFlagBits::DepthNonInclusive ? ERglEnum::Greater : ERglEnum::GEqual;
  } else {
    func = modelFlags & CModelFlagBits::DepthNonInclusive ? ERglEnum::Less : ERglEnum::LEqual;
  }
  bool depthWrite = modelFlags & CModelFlagBits::DepthUpdate && matFlags & CCubeMaterialFlagBits::fDepthWrite;
  aurora::gfx::set_depth_mode(true, func, depthWrite);
}

void CCubeMaterial::ResetCachedMaterials() {
  KillCachedViewDepState();
  sLastMaterialUnique = UINT32_MAX;
  sRenderingModel = nullptr;
  sLastMaterialCached = nullptr;
}

void CCubeMaterial::KillCachedViewDepState() { sLastModelCached = nullptr; }

void CCubeMaterial::EnsureViewDepStateCached(const CCubeSurface* surface) {
  // TODO
  if ((surface != nullptr || sLastModelCached != sRenderingModel) && sRenderingModel != nullptr) {
    sLastModelCached = sRenderingModel;
    if (surface == nullptr) {
      sReflectionType = 1;
    } else {
      sReflectionType = 2;
    }
    if (g_Renderer->IsReflectionDirty()) {

    } else {
      g_Renderer->SetReflectionDirty(true);
    }
  }
}

u32 CCubeMaterial::HandleColorChannels(u32 chanCount, u32 firstChan) {
  if (CCubeModel::sRenderModelShadow) {
    if (chanCount != 0) {
      aurora::gfx::set_chan_amb_color(GX::COLOR1A1, zeus::skBlack);
      aurora::gfx::set_chan_mat_color(GX::COLOR1A1, zeus::skWhite);

      // TODO chan / lights

      aurora::gfx::set_chan_mat_color(GX::COLOR0A0, zeus::skWhite);
    }
    return 2;
  }

  if (chanCount == 2) {
    aurora::gfx::set_chan_amb_color(GX::COLOR1A1, zeus::skBlack);
    aurora::gfx::set_chan_mat_color(GX::COLOR1A1, zeus::skWhite);
  } else {
    // TODO chan ctrls
  }

  if (chanCount == 0) {
    // TODO more chan ctrls
  } else {
    auto uVar3 = firstChan & 0xfffffffe;
    // TODO enabled lights
    // TODO chan ctrl
    // if (sEnabledLights == 0) {
    //   aurora::gfx::set_chan_mat_color(GX::COLOR0A0, amb_clr);
    // } else {
    aurora::gfx::set_chan_mat_color(GX::COLOR0A0, zeus::skWhite);
    // }
  }

  // TODO
  aurora::gfx::set_chan_mat_src(GX::COLOR0A0, GX::SRC_REG);
  aurora::gfx::set_chan_mat_src(GX::COLOR1A1, GX::SRC_REG);
  return chanCount;
}

void CCubeMaterial::HandleTev(u32 tevCur, const u32* materialDataCur, const u32* texMapTexCoordFlags,
                              bool shadowMapsEnabled) {
  const u32 colorArgs = shadowMapsEnabled ? 0x7a04f : SBig(materialDataCur[0]);
  const u32 alphaArgs = SBig(materialDataCur[1]);
  const u32 colorOps = SBig(materialDataCur[2]);
  const u32 alphaOps = SBig(materialDataCur[3]);

  // GXSetTevDirect(tevCur);
  const CTevCombiners::ColorPass colPass{colorArgs};
  const CTevCombiners::AlphaPass alphaPass{alphaArgs};
  const CTevCombiners::CTevOp colorOp{colorOps};
  const CTevCombiners::CTevOp alphaOp{alphaOps};
  // TODO does this actually change anything?
  // if (colorOps == alphaOps && ((colorOps & 0x1FF) == 0x100)) {
  //  colorOp = {true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, colorOp.x10_regId};
  //  alphaOp = {true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, colorOp.x10_regId};
  // }
  aurora::gfx::update_tev_stage(static_cast<ERglTevStage>(tevCur), colPass, alphaPass, colorOp, alphaOp);

  u32 tmtcFlags = SBig(*texMapTexCoordFlags);
  u32 matFlags = SBig(materialDataCur[4]);
  aurora::gfx::set_tev_order(static_cast<GX::TevStageID>(tevCur), static_cast<GX::TexCoordID>(tmtcFlags & 0xFF),
                             static_cast<GX::TexMapID>(tmtcFlags >> 8 & 0xFF),
                             static_cast<GX::ChannelID>(matFlags & 0xFF));
  aurora::gfx::set_tev_k_color_sel(static_cast<GX::TevStageID>(tevCur),
                                   static_cast<GX::TevKColorSel>(matFlags >> 0x8 & 0xFF));
  aurora::gfx::set_tev_k_alpha_sel(static_cast<GX::TevStageID>(tevCur),
                                   static_cast<GX::TevKAlphaSel>(matFlags >> 0x10 & 0xFF));
}

u32 CCubeMaterial::HandleAnimatedUV(const u32* uvAnim, u32 texMtx, u32 pttTexMtx) {
  u32 type = SBig(*uvAnim);
  switch (type) {
  case 0:
    // TODO
    return 1;
  case 1:
    // TODO
    return 1;
  case 2:
    // TODO
    return 5;
  case 3:
    // TODO
    return 3;
  case 4:
  case 5:
    // TODO
    return 5;
  case 6:
    // TODO
    return 1;
  case 7:
    // TODO
    return 2;
  default:
    return 0;
  }
}

void CCubeMaterial::HandleTransparency(u32& finalTevCount, u32& finalKColorCount, const CModelFlags& modelFlags,
                                       u32 blendFactors, u32& finalCCFlags, u32& finalACFlags) {
  if (modelFlags.x0_blendMode == 2) {
    u16 dstFactor = blendFactors >> 16 & 0xffff;
    if (dstFactor == 1)
      return;
  }
  if (modelFlags.x0_blendMode == 3) {
    // Stage outputting splatted KAlpha as color to reg0
    aurora::gfx::update_tev_stage(static_cast<ERglTevStage>(finalTevCount),
                                  CTevCombiners::ColorPass{GX::CC_ZERO, GX::CC_ZERO, GX::CC_ZERO, GX::CC_KONST},
                                  CTevCombiners::AlphaPass{GX::CA_ZERO, GX::CA_ZERO, GX::CA_ZERO, GX::CA_APREV},
                                  CTevCombiners::CTevOp{true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, GX::TEVREG0},
                                  CTevCombiners::CTevOp{true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, GX::TEVPREV});
    // GXSetTevColorIn(finalTevCount, TEVCOLORARG_ZERO, TEVCOLORARG_ZERO, TEVCOLORARG_ZERO, TEVCOLORARG_KONST);
    // GXSetTevAlphaIn(finalTevCount, TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_APREV);
    // GXSetTevColorOp(finalTevCount, 0, 0, 0, 1, 1); // ColorReg0
    aurora::gfx::set_tev_k_color_sel(static_cast<GX::TevStageID>(finalTevCount),
                                     static_cast<GX::TevKColorSel>(finalKColorCount + GX::TEV_KCSEL_K0_A));
    // GXSetTevKColorSel(finalTevCount, finalKColorCount+28);
    // GXSetTevAlphaOp(finalTevCount, 0, 0, 0, 1, 0); // AlphaRegPrev
    // GXSetTevOrder(finalTevCount, 255, 255, 255);
    // GXSetTevDirect(finalTevCount);
    // Stage interpolating from splatted KAlpha using KColor
    aurora::gfx::update_tev_stage(static_cast<ERglTevStage>(finalTevCount + 1),
                                  CTevCombiners::ColorPass{GX::CC_CPREV, GX::CC_C0, GX::CC_KONST, GX::CC_ZERO},
                                  CTevCombiners::AlphaPass{GX::CA_ZERO, GX::CA_ZERO, GX::CA_ZERO, GX::CA_APREV},
                                  CTevCombiners::CTevOp{true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, GX::TEVPREV},
                                  CTevCombiners::CTevOp{true, GX::TEV_ADD, GX::TB_ZERO, GX::CS_SCALE_1, GX::TEVPREV});
    // GXSetTevColorIn(finalTevCount + 1, TEVCOLORARG_CPREV, TEVCOLORARG_C0, TEVCOLORARG_KONST, TEVCOLORARG_ZERO);
    // GXSetTevAlphaIn(finalTevCount + 1, TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_APREV);
    aurora::gfx::set_tev_k_color_sel(static_cast<GX::TevStageID>(finalTevCount + 1),
                                     static_cast<GX::TevKColorSel>(finalKColorCount + GX::TEV_KCSEL_K0));
    // GXSetTevKColorSel(finalTevCount, finalKColorCount+12);
    // SetStandardTevColorAlphaOp(finalTevCount + 1);
    // GXSetTevDirect(finalTevCount + 1);
    // GXSetTevOrder(finalTevCount + 1, 255, 255, 255);
    aurora::gfx::set_tev_k_color(static_cast<GX::TevKColorID>(finalKColorCount), modelFlags.x4_color);
    // GXSetTevKColor(finalKColorCount, modelFlags.x4_color);
    finalKColorCount += 1;
    finalTevCount += 2;
  } else {
    // Mul KAlpha
    CTevCombiners::AlphaPass alphaPass{GX::CA_ZERO, GX::CA_KONST, GX::CA_APREV, GX::CA_ZERO};
    u32 tevAlpha = 0x000380C7; // TEVALPHAARG_ZERO, TEVALPHAARG_KONST, TEVALPHAARG_APREV, TEVALPHAARG_ZERO
    if (modelFlags.x0_blendMode == 8) {
      // Set KAlpha
      alphaPass = {GX::CA_ZERO, GX::CA_ZERO, GX::CA_ZERO, GX::CA_KONST};
      tevAlpha = 0x00031CE7; // TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_ZERO, TEVALPHAARG_KONST
    }
    // Mul KColor
    CTevCombiners::ColorPass colorPass{GX::CC_ZERO, GX::CC_KONST, GX::CC_CPREV, GX::CC_ZERO};
    u32 tevColor = 0x000781CF; // TEVCOLORARG_ZERO, TEVCOLORARG_KONST, TEVCOLORARG_CPREV, TEVCOLORARG_ZERO
    if (modelFlags.x0_blendMode == 2) {
      // Add KColor
      colorPass = {GX::CC_ZERO, GX::CC_ONE, GX::CC_CPREV, GX::CC_KONST};
      tevColor = 0x0007018F; // TEVCOLORARG_ZERO, TEVCOLORARG_ONE, TEVCOLORARG_CPREV, TEVCOLORARG_KONST
    }
    aurora::gfx::update_tev_stage(static_cast<ERglTevStage>(finalTevCount), colorPass, alphaPass, {}, {});
    // GXSetTevColorIn(finalTevCount)
    // GXSetTevAlphaIn(finalTevCount)
    // SetStandardTevColorAlphaOp(finalTevCount);
    finalCCFlags = 0x100; // Just clamp, output prev reg
    finalACFlags = 0x100;
    // GXSetTevDirect(finalTevCount);
    // GXSetTevOrder(finalTevCount, 255, 255, 255);
    aurora::gfx::set_tev_k_color(static_cast<GX::TevKColorID>(finalKColorCount), modelFlags.x4_color);
    // GXSetTevKColor(finalKColorCount, modelFlags.x4_color);
    aurora::gfx::set_tev_k_color_sel(static_cast<GX::TevStageID>(finalKColorCount),
                                     static_cast<GX::TevKColorSel>(finalKColorCount + GX::TEV_KCSEL_K0));
    // GXSetTevKColorSel(finalTevCount, finalKColorCount+12);
    aurora::gfx::set_tev_k_alpha_sel(static_cast<GX::TevStageID>(finalTevCount),
                                     static_cast<GX::TevKAlphaSel>(finalKColorCount + GX::TEV_KASEL_K0_A));
    // GXSetTevKAlphaSel(finalTevCount, finalKColorCount+28);
    finalTevCount += 1;
    finalKColorCount += 1;
  }
}

u32 CCubeMaterial::HandleReflection(bool usesTevReg2, u32 indTexSlot, u32 r5, u32 finalTevCount, u32 texCount,
                                    u32 tcgCount, u32 finalKColorCount, u32& finalCCFlags, u32& finalACFlags) {
  u32 out = 0;
  if (usesTevReg2) {
    // GX_CC_C2
    out = 1;
  } else {
    // GX_CC_KONST
  }
  aurora::gfx::set_tev_k_color(static_cast<GX::TevKColorID>(finalKColorCount),
                               zeus::CColor{sReflectionAlpha, sReflectionAlpha});
  aurora::gfx::set_tev_k_color_sel(static_cast<GX::TevStageID>(finalTevCount),
                                   static_cast<GX::TevKColorSel>(GX::TEV_KCSEL_K0 + finalKColorCount));

  const auto stage = static_cast<GX::TevStageID>(finalTevCount + out);
  // tex = g_Renderer->GetRealReflection
  // tex.Load(texCount, 0)

  // TODO

  finalACFlags = 0;
  finalCCFlags = 0;

  // aurora::gfx::set_tev_order(stage, ...)

  return out + 1;
}

void CCubeMaterial::DoPassthru(u32 finalTevCount) {
  aurora::gfx::disable_tev_stage(static_cast<ERglTevStage>(finalTevCount));
}

void CCubeMaterial::DoModelShadow(u32 texCount, u32 tcgCount) {
  // CCubeModel::sShadowTexture->Load(texCount, EClampMode::One);
  // TODO
}

static GX::TevStageID sCurrentTevStage = GX::NULL_STAGE;
void CCubeMaterial::EnsureTevsDirect() {
  if (sCurrentTevStage == GX::NULL_STAGE) {
    return;
  }

  // CGX::SetNumIndStages(0);
  // CGX::SetTevDirect(sCurrentTevStage);
  sCurrentTevStage = GX::NULL_STAGE;
}
} // namespace metaforce