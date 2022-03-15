#pragma once

#include "../common.hpp"
#include "../gx.hpp"

namespace aurora::gfx::stream {
struct DrawData {
  PipelineRef pipeline;
  Range vertRange;
  Range uniformRange;
  uint32_t vertexCount;
  gx::GXBindGroups bindGroups;
};

struct PipelineConfig : public gx::PipelineConfig {};

struct State {};

State construct_state();
wgpu::RenderPipeline create_pipeline(const State& state, [[maybe_unused]] PipelineConfig config);
void render(const State& state, const DrawData& data, const wgpu::RenderPassEncoder& pass);
} // namespace aurora::gfx::stream
