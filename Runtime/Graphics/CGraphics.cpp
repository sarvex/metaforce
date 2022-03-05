#include "Runtime/Graphics/CGraphics.hpp"

#include "Runtime/CTimeProvider.hpp"
#include "Runtime/Graphics/CLight.hpp"
#include "Runtime/Graphics/CLineRenderer.hpp"
#include "Runtime/Graphics/CTexture.hpp"
#include "Runtime/Graphics/Shaders/CTextSupportShader.hpp"
#include "Runtime/GuiSys/CGuiSys.hpp"

#include <zeus/Math.hpp>

namespace metaforce {
CGraphics::CProjectionState CGraphics::g_Proj;
CFogState CGraphics::g_Fog;
std::array<zeus::CColor, 3> CGraphics::g_ColorRegs{};
float CGraphics::g_ProjAspect = 1.f;
u32 CGraphics::g_NumLightsActive = 0;
u32 CGraphics::g_NumBreakpointsWaiting = 0;
u32 CGraphics::g_FlippingState;
bool CGraphics::g_LastFrameUsedAbove = false;
bool CGraphics::g_InterruptLastFrameUsedAbove = false;
ERglLightBits CGraphics::g_LightActive = ERglLightBits::None;
ERglLightBits CGraphics::g_LightsWereOn = ERglLightBits::None;
zeus::CTransform CGraphics::g_GXModelView;
zeus::CTransform CGraphics::g_GXModelViewInvXpose;
zeus::CTransform CGraphics::g_GXModelMatrix = zeus::CTransform();
zeus::CTransform CGraphics::g_ViewMatrix;
zeus::CVector3f CGraphics::g_ViewPoint;
zeus::CTransform CGraphics::g_GXViewPointMatrix;
zeus::CTransform CGraphics::g_CameraMatrix;
SClipScreenRect CGraphics::g_CroppedViewport;
bool CGraphics::g_IsGXModelMatrixIdentity = true;
zeus::CColor CGraphics::g_ClearColor = zeus::skClear;
float CGraphics::g_ClearDepthValue = 1.f;
bool CGraphics::g_IsBeginSceneClearFb = true;

SViewport CGraphics::g_Viewport = {
    0, 0, 640, 480, 640 / 2.f, 480 / 2.f, 0.0f,
};
u32 CGraphics::g_FrameCounter = 0;
u32 CGraphics::g_Framerate = 0;
u32 CGraphics::g_FramesPast = 0;
frame_clock::time_point CGraphics::g_FrameStartTime = frame_clock::now();
ERglEnum CGraphics::g_depthFunc = ERglEnum::Never;
ERglCullMode CGraphics::g_cullMode = ERglCullMode::None;

const std::array<zeus::CMatrix3f, 6> CGraphics::skCubeBasisMats{{
    /* Right */
    {0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f},
    /* Left */
    {0.f, -1.f, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f, -1.f},
    /* Up */
    {1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 1.f, 0.f},
    /* Down */
    {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f},
    /* Back */
    {1.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, -1.f},
    /* Forward */
    {-1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, -1.f},
}};

void CGraphics::DisableAllLights() {
  g_NumLightsActive = 0;
  g_LightActive = ERglLightBits::None;
  // TODO: turn lights off for real
}

void CGraphics::LoadLight(ERglLight light, const CLight& info) {
  // TODO: load light for real
}

void CGraphics::EnableLight(ERglLight light) {
  ERglLightBits lightBit = ERglLightBits(1 << int(light));
  if ((lightBit & g_LightActive) == ERglLightBits::None) {
    g_LightActive |= lightBit;
    ++g_NumLightsActive;
    // TODO: turn light on for real
  }
  g_LightsWereOn = g_LightActive;
}

void CGraphics::SetLightState(ERglLightBits lightState) {
  // TODO: set state for real
  g_LightActive = lightState;
  g_NumLightsActive = zeus::PopCount(lightState);
}

void CGraphics::SetAmbientColor(const zeus::CColor& col) {
  // TODO: set for real
}

void CGraphics::SetFog(ERglFogMode mode, float startz, float endz, const zeus::CColor& color) {
  g_Fog.m_mode = mode > ERglFogMode::PerspRevExp2 ? ERglFogMode(int(mode) - 8) : mode;
  g_Fog.m_color = color;
  if (CGraphics::g_Proj.x18_far == CGraphics::g_Proj.x14_near || endz == startz) {
    g_Fog.m_A = 0.f;
    g_Fog.m_B = 0.5f;
    g_Fog.m_C = 0.f;
  } else {
    float depthrange = CGraphics::g_Proj.x18_far - CGraphics::g_Proj.x14_near;
    float fogrange = endz - startz;
    g_Fog.m_A = (CGraphics::g_Proj.x18_far * CGraphics::g_Proj.x14_near) / (depthrange * fogrange);
    g_Fog.m_B = CGraphics::g_Proj.x18_far / depthrange;
    g_Fog.m_C = startz / fogrange;
  }
}

void CGraphics::SetDepthWriteMode(bool compare_enable, ERglEnum comp, bool update_enable) {
  g_depthFunc = comp;
  aurora::gfx::set_depth_mode(compare_enable, comp, update_enable);
}

void CGraphics::SetBlendMode(ERglBlendMode mode, ERglBlendFactor src, ERglBlendFactor dst, ERglLogicOp op) {
  aurora::gfx::set_blend_mode(mode, src, dst, op);
}

void CGraphics::SetCullMode(ERglCullMode mode) {
  g_cullMode = mode;
  aurora::gfx::set_cull_mode(mode);
}

void CGraphics::BeginScene() {
  // ClearBackAndDepthBuffers();
}

void CGraphics::EndScene() {
  aurora::gfx::set_depth_mode(true, ERglEnum::LEqual, true);
  /* Spinwait until g_NumBreakpointsWaiting is 0 */
  /* ++g_NumBreakpointsWaiting; */
  /* GXCopyDisp to g_CurrenFrameBuf with clear enabled */
  /* Register next breakpoint with GP FIFO */

  /* Yup, GX effectively had fences long before D3D12 and Vulkan
   * (same functionality implemented in boo's execute method) */

  /* This usually comes from VI register during interrupt;
   * we don't care in the era of progressive-scan dominance,
   * so simulate field-flipping with XOR instead */
  g_InterruptLastFrameUsedAbove ^= 1;
  g_LastFrameUsedAbove = g_InterruptLastFrameUsedAbove;

  /* Flush text instance buffers just before GPU command list submission */
  CTextSupportShader::UpdateBuffers();

  /* Same with line renderer */
  //  CLineRenderer::UpdateBuffers();

  ++g_FrameCounter;

  UpdateFPSCounter();
}

void CGraphics::Render2D(const CTexture& tex, u32 x, u32 y, u32 w, u32 h, const zeus::CColor& col) {
  const auto oldProj = g_Proj;
  CGraphics::SetOrtho(-g_Viewport.x8_width / 2, g_Viewport.x8_width / 2, g_Viewport.xc_height / 2,
                      -g_Viewport.xc_height / 2, -1.f, -10.f);
  // TODO
  g_Proj = oldProj;
  FlushProjection();
}

bool CGraphics::BeginRender2D(const CTexture& tex) { return false; }

void CGraphics::DoRender2D(const CTexture& tex, s32 x, s32 y, s32 w1, s32 w2, s32 w3, s32 w4, s32 w5,
                           const zeus::CColor& col) {}

void CGraphics::EndRender2D(bool v) {}

void CGraphics::SetAlphaCompare(ERglAlphaFunc comp0, u8 ref0, ERglAlphaOp op, ERglAlphaFunc comp1, u8 ref1) {}

void CGraphics::SetViewPointMatrix(const zeus::CTransform& xf) {
  g_ViewMatrix = xf;
  g_ViewPoint = xf.origin;
  zeus::CMatrix3f tmp(xf.basis[0], xf.basis[2], -xf.basis[1]);
  g_GXViewPointMatrix = zeus::CTransform(tmp.transposed());
  SetViewMatrix();
}

void CGraphics::SetViewMatrix() {
  g_CameraMatrix = g_GXViewPointMatrix * zeus::CTransform::Translate(-g_ViewPoint);
  if (g_IsGXModelMatrixIdentity)
    g_GXModelView = g_CameraMatrix;
  else
    g_GXModelView = g_CameraMatrix * g_GXModelMatrix;
  /* Load position matrix */
  /* Inverse-transpose */
  g_GXModelViewInvXpose = g_GXModelView.inverse();
  g_GXModelViewInvXpose.origin.zeroOut();
  g_GXModelViewInvXpose.basis.transpose();
  /* Load normal matrix */
  aurora::gfx::update_model_view(g_GXModelView.toMatrix4f(), g_GXModelViewInvXpose.toMatrix4f());
}

void CGraphics::SetModelMatrix(const zeus::CTransform& xf) {
  g_IsGXModelMatrixIdentity = false;
  g_GXModelMatrix = xf;
  SetViewMatrix();
}

constexpr zeus::CMatrix4f PlusOneZ(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f);

constexpr zeus::CMatrix4f VulkanCorrect(1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, 0.f, 0.5f, 0.5f + FLT_EPSILON,
                                        0.f, 0.f, 0.f, 1.f);

zeus::CMatrix4f CGraphics::CalculatePerspectiveMatrix(float fovy, float aspect, float znear, float zfar) {
  CProjectionState st;
  float tfov = std::tan(zeus::degToRad(fovy * 0.5f));
  st.x14_near = znear;
  st.x18_far = zfar;
  st.xc_top = znear * tfov;
  st.x10_bottom = -st.xc_top;
  st.x8_right = aspect * znear * tfov;
  st.x4_left = -st.x8_right;

  float rml = st.x8_right - st.x4_left;
  float rpl = st.x8_right + st.x4_left;
  float tmb = st.xc_top - st.x10_bottom;
  float tpb = st.xc_top + st.x10_bottom;
  float fpn = st.x18_far + st.x14_near;
  float fmn = st.x18_far - st.x14_near;
  // clang-format off
  return {
      2.f * st.x14_near / rml, 0.f, rpl / rml, 0.f,
      0.f, 2.f * st.x14_near / tmb, tpb / tmb, 0.f,
      0.f, 0.f, -fpn / fmn, -2.f * st.x18_far * st.x14_near / fmn,
      0.f, 0.f, -1.f, 0.f,
  };
  // clang-format on
}

zeus::CMatrix4f CGraphics::GetPerspectiveProjectionMatrix() {
  if (g_Proj.x0_persp) {
    float rml = g_Proj.x8_right - g_Proj.x4_left;
    float rpl = g_Proj.x8_right + g_Proj.x4_left;
    float tmb = g_Proj.xc_top - g_Proj.x10_bottom;
    float tpb = g_Proj.xc_top + g_Proj.x10_bottom;
    float fpn = g_Proj.x18_far + g_Proj.x14_near;
    float fmn = g_Proj.x18_far - g_Proj.x14_near;
    // clang-format off
    return {
        2.f * g_Proj.x14_near / rml, 0.f, rpl / rml, 0.f, 0.f,
        2.f * g_Proj.x14_near / tmb, tpb / tmb, 0.f,
        0.f, 0.f, -fpn / fmn, -2.f * g_Proj.x18_far * g_Proj.x14_near / fmn,
        0.f, 0.f, -1.f, 0.f,
    };
    // clang-format on
  } else {
    float rml = g_Proj.x8_right - g_Proj.x4_left;
    float rpl = g_Proj.x8_right + g_Proj.x4_left;
    float tmb = g_Proj.xc_top - g_Proj.x10_bottom;
    float tpb = g_Proj.xc_top + g_Proj.x10_bottom;
    float fpn = g_Proj.x18_far + g_Proj.x14_near;
    float fmn = g_Proj.x18_far - g_Proj.x14_near;
    // clang-format off
    return {
        2.f / rml, 0.f, 0.f, -rpl / rml,
        0.f, 2.f / tmb, 0.f, -tpb / tmb,
        0.f, 0.f, -2.f / fmn, -fpn / fmn,
        0.f, 0.f, 0.f, 1.f
    };
    // clang-format on
  }
}

const CGraphics::CProjectionState& CGraphics::GetProjectionState() { return g_Proj; }

void CGraphics::SetProjectionState(const CGraphics::CProjectionState& proj) {
  g_Proj = proj;
  FlushProjection();
}

void CGraphics::SetPerspective(float fovy, float aspect, float znear, float zfar) {
  g_ProjAspect = aspect;

  float tfov = std::tan(zeus::degToRad(fovy * 0.5f));
  g_Proj.x0_persp = true;
  g_Proj.x14_near = znear;
  g_Proj.x18_far = zfar;
  g_Proj.xc_top = znear * tfov;
  g_Proj.x10_bottom = -g_Proj.xc_top;
  g_Proj.x8_right = aspect * znear * tfov;
  g_Proj.x4_left = -g_Proj.x8_right;

  FlushProjection();
}

void CGraphics::SetOrtho(float left, float right, float top, float bottom, float znear, float zfar) {
  g_Proj.x0_persp = false;
  g_Proj.x4_left = left;
  g_Proj.x8_right = right;
  g_Proj.xc_top = top;
  g_Proj.x10_bottom = bottom;
  g_Proj.x14_near = znear;
  g_Proj.x18_far = zfar;

  FlushProjection();
}

void CGraphics::FlushProjection() {
  if (g_Proj.x0_persp) {
    // Convert and load persp
  } else {
    // Convert and load ortho
  }
  aurora::gfx::update_projection(GetPerspectiveProjectionMatrix());
}

zeus::CVector2i CGraphics::ProjectPoint(const zeus::CVector3f& point) {
  zeus::CVector3f projPt = GetPerspectiveProjectionMatrix().multiplyOneOverW(point);
  return {int(projPt.x() * g_Viewport.x10_halfWidth) + int(g_Viewport.x10_halfWidth),
          int(g_Viewport.xc_height) - (int(projPt.y() * g_Viewport.x14_halfHeight) + int(g_Viewport.x14_halfHeight))};
}

SClipScreenRect CGraphics::ClipScreenRectFromMS(const zeus::CVector3f& p1, const zeus::CVector3f& p2) {
  zeus::CVector3f xf1 = g_GXModelView * p1;
  zeus::CVector3f xf2 = g_GXModelView * p2;
  return ClipScreenRectFromVS(xf1, xf2);
}

SClipScreenRect CGraphics::ClipScreenRectFromVS(const zeus::CVector3f& p1, const zeus::CVector3f& p2) {
  if (p1.x() == 0.f && p1.y() == 0.f && p1.z() == 0.f)
    return {};
  if (p2.x() == 0.f && p2.y() == 0.f && p2.z() == 0.f)
    return {};

  if (-p1.z() < GetProjectionState().x14_near || -p2.z() < GetProjectionState().x14_near)
    return {};
  if (-p1.z() > GetProjectionState().x18_far || -p2.z() > GetProjectionState().x18_far)
    return {};

  zeus::CVector2i sp1 = ProjectPoint(p1);
  zeus::CVector2i sp2 = ProjectPoint(p2);
  int minX = std::min(sp2.x, sp1.x);
  int minX2 = minX & 0xfffffffe;
  int minY = std::min(sp2.y, sp1.y);
  int minY2 = minY & 0xfffffffe;

  if (minX2 >= g_Viewport.x8_width)
    return {};

  int maxX = abs(sp1.x - sp2.x) + minX;
  int maxX2 = (maxX + 2) & 0xfffffffe;
  if (maxX2 <= 0 /* ViewportX origin */)
    return {};

  // int finalMinX = std::max(minX, 0 /* ViewportX origin */);
  // int finalMaxX = std::min(maxX, int(g_Viewport.x8_width));

  if (minY2 >= g_Viewport.xc_height)
    return {};

  int maxY = abs(sp1.y - sp2.y) + minY;
  int maxY2 = (maxY + 2) & 0xfffffffe;
  if (maxY2 <= 0 /* ViewportY origin */)
    return {};

  // int finalMinY = std::max(minY, 0 /* ViewportY origin */);
  // int finalMaxY = std::min(maxY, int(g_Viewport.xc_height));

  int width = maxX2 - minX2;
  int height = maxY2 - minY2;
  return {true,
          minX2,
          minY2,
          width,
          height,
          width,
          minX2 / float(g_Viewport.x8_width),
          maxX2 / float(g_Viewport.x8_width),
          1.f - maxY2 / float(g_Viewport.xc_height),
          1.f - minY2 / float(g_Viewport.xc_height)};
}

void CGraphics::SetViewportResolution(const zeus::CVector2i& res) {
  g_Viewport.x8_width = res.x;
  g_Viewport.xc_height = res.y;
  g_CroppedViewport = SClipScreenRect();
  g_CroppedViewport.xc_width = res.x;
  g_CroppedViewport.x10_height = res.y;
  g_Viewport.x10_halfWidth = res.x / 2.f;
  g_Viewport.x14_halfHeight = res.y / 2.f;
  g_Viewport.aspect = res.x / float(res.y);
  if (g_GuiSys)
    g_GuiSys->OnViewportResize();
}

static zeus::CRectangle CachedVP;
zeus::CVector2f CGraphics::g_CachedDepthRange = {0.f, 1.f};

void CGraphics::SetViewport(int leftOff, int bottomOff, int width, int height) {
  CachedVP.position[0] = leftOff;
  CachedVP.position[1] = bottomOff;
  CachedVP.size[0] = width;
  CachedVP.size[1] = height;
  aurora::gfx::set_viewport(CachedVP, g_CachedDepthRange[0], g_CachedDepthRange[1]);
}

void CGraphics::SetScissor(int leftOff, int bottomOff, int width, int height) {
  aurora::gfx::set_scissor(leftOff, bottomOff, width, height);
}

void CGraphics::SetDepthRange(float znear, float zfar) {
  g_CachedDepthRange[0] = znear;
  g_CachedDepthRange[1] = zfar;
  aurora::gfx::set_viewport(CachedVP, g_CachedDepthRange[0], g_CachedDepthRange[1]);
}

CTimeProvider* CGraphics::g_ExternalTimeProvider = nullptr;
float CGraphics::g_DefaultSeconds = 0.f;
u32 CGraphics::g_RenderTimings = 0;

float CGraphics::GetSecondsMod900() {
  if (!g_ExternalTimeProvider)
    return g_DefaultSeconds;
  return g_ExternalTimeProvider->x0_currentTime;
}

void CGraphics::TickRenderTimings() {
  OPTICK_EVENT();
  g_RenderTimings = (g_RenderTimings + 1) % u32(900 * 60);
  g_DefaultSeconds = g_RenderTimings / 60.f;
}

static constexpr u64 FPS_REFRESH_RATE = 1000;
void CGraphics::UpdateFPSCounter() {
  ++g_FramesPast;

  std::chrono::duration<double, std::milli> timeElapsed = frame_clock::now() - g_FrameStartTime;
  if (timeElapsed.count() > FPS_REFRESH_RATE) {
    g_Framerate = g_FramesPast;
    g_FrameStartTime = frame_clock::now();
    g_FramesPast = 0;
  }
}

static bool g_UseVideoFilter = false;
void CGraphics::SetUseVideoFilter(bool filter) {
  g_UseVideoFilter = filter;
  // GXSetCopyFilter(CGraphics::mRenderModeObj.aa, CGraphics::mRenderModeObj.sample_pattern, filter,
  //                 CGraphics::mRenderModeObj.vfilter);
}

void CGraphics::SetClearColor(const zeus::CColor& color) {
  g_ClearColor = color;
  aurora::gfx::set_clear_color(color);
}

void CGraphics::SetCopyClear(const zeus::CColor& color, float depth) {
  g_ClearColor = color;
  g_ClearDepthValue = depth; // 1.6777215E7 * depth; Metroid Prime needed this to convert float [0,1] depth into 24 bit
                             // range, we no longer have this requirement
  aurora::gfx::set_clear_color(color);
  // TODO do we care about depth value?
  // GXSetCopyClear(g_ClearColor, g_ClearDepthValue);
}

void CGraphics::SetIsBeginSceneClearFb(bool clear) { g_IsBeginSceneClearFb = clear; }

// Stream API
static EStreamFlags sStreamFlags;
static zeus::CColor sQueuedColor;
static zeus::CVector2f sQueuedTexCoord;
static zeus::CVector3f sQueuedNormal;

void CGraphics::SetTevOp(ERglTevStage stage, const CTevCombiners::CTevPass& pass) {
  CTevCombiners::SetupPass(stage, pass);
}

void CGraphics::StreamBegin(GX::Primitive primitive) {
  sStreamFlags = {};
  aurora::gfx::stream_begin(primitive);
}

void CGraphics::StreamNormal(const zeus::CVector3f& nrm) {
  sQueuedNormal = nrm;
  sStreamFlags |= EStreamFlagBits::fHasNormal;
}

void CGraphics::StreamColor(float r, float g, float b, float a) {
  sQueuedColor = zeus::CColor{r, g, b, a};
  sStreamFlags |= EStreamFlagBits::fHasColor;
}

void CGraphics::StreamColor(const zeus::CColor& color) {
  sQueuedColor = color;
  sStreamFlags |= EStreamFlagBits::fHasColor;
}

void CGraphics::StreamTexcoord(float x, float y) {
  sQueuedTexCoord = {x, y};
  sStreamFlags |= EStreamFlagBits::fHasTexture;
}

void CGraphics::StreamTexcoord(const zeus::CVector2f& uv) {
  sQueuedTexCoord = uv;
  sStreamFlags |= EStreamFlagBits::fHasTexture;
}

void CGraphics::StreamVertex(float xyz) {
  const zeus::CVector3f pos{xyz, xyz, xyz};
  aurora::gfx::stream_vertex(sStreamFlags, pos, sQueuedNormal, sQueuedColor, sQueuedTexCoord);
}

void CGraphics::StreamVertex(float x, float y, float z) {
  const zeus::CVector3f pos{x, y, z};
  aurora::gfx::stream_vertex(sStreamFlags, pos, sQueuedNormal, sQueuedColor, sQueuedTexCoord);
}

void CGraphics::StreamVertex(const zeus::CVector3f& pos) {
  aurora::gfx::stream_vertex(sStreamFlags, pos, sQueuedNormal, sQueuedColor, sQueuedTexCoord);
}

void CGraphics::StreamEnd() { aurora::gfx::stream_end(); }

void CGraphics::DrawPrimitive(GX::Primitive primitive, const zeus::CVector3f* pos, const zeus::CVector3f& normal,
                              const zeus::CColor& col, s32 numVerts) {
  StreamBegin(primitive);
  StreamColor(col);
  StreamNormal(normal);
  for (u32 i = 0; i < numVerts; ++i) {
    StreamVertex(pos[i]);
  }
  StreamEnd();
}
} // namespace metaforce
