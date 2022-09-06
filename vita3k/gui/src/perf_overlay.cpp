// Vita3K emulator project
// Copyright (C) 2024 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "private.h"

#include <config/state.h>

namespace gui {
static const ImVec2 PERF_OVERLAY_PAD = ImVec2(12.f, 12.f);
static const ImVec4 PERF_OVERLAY_BG_COLOR = ImVec4(0.282f, 0.239f, 0.545f, 0.8f);

static ImVec2 get_perf_pos(ImVec2 window_size, EmuEnvState &emuenv, ImVec2 scale) {
    const float TOP = PERF_OVERLAY_PAD.y * scale.y;
    const float LEFT = PERF_OVERLAY_PAD.x * scale.x;
    const float CENTER = ImGui::GetIO().DisplaySize.x / 2.0 - (window_size.x / 2.f);
    const float RIGHT = ImGui::GetIO().DisplaySize.x - window_size.x + PERF_OVERLAY_PAD.x * scale.x;
    const float BOTTOM = ImGui::GetIO().DisplaySize.y - window_size.y + PERF_OVERLAY_PAD.y * scale.y;

    switch (emuenv.cfg.performance_overlay_position) {
    case TOP_CENTER: return ImVec2(CENTER, TOP);
    case TOP_RIGHT: return ImVec2(RIGHT, TOP);
    case BOTTOM_LEFT: return ImVec2(LEFT, BOTTOM);
    case BOTTOM_CENTER: return ImVec2(CENTER, BOTTOM);
    case BOTTOM_RIGHT: return ImVec2(RIGHT, BOTTOM);
    case TOP_LEFT:
    default: break;
    }

    return ImVec2(LEFT, TOP);
}

static float get_perf_height(EmuEnvState &emuenv) {
    switch (emuenv.cfg.performance_overlay_detail) {
    case MAXIMUM: return 143.f;
    case MEDIUM: return 85.f;
    case LOW:
    case MINIMUM:
    default: break;
    }

    return 62.f;
}

void draw_perf_overlay(GuiState &gui, EmuEnvState &emuenv) {
    auto lang = gui.lang.performance_overlay;

    const ImVec2 display_size(emuenv.viewport_size.x, emuenv.viewport_size.y);
    const auto RES_SCALE = ImVec2(display_size.x / emuenv.res_width_dpi_scale, display_size.y / emuenv.res_height_dpi_scale);
    const auto SCALE = ImVec2(RES_SCALE.x * emuenv.dpi_scale, RES_SCALE.y * emuenv.dpi_scale);

    const auto MAIN_WINDOW_SIZE = ImVec2((emuenv.cfg.performance_overlay_detail == MINIMUM ? 105.5f : 162.f) * SCALE.x, get_perf_height(emuenv) * SCALE.y);

    const auto WINDOW_POS = get_perf_pos(MAIN_WINDOW_SIZE, emuenv, SCALE);
    const auto WINDOW_SIZE = ImVec2((emuenv.cfg.performance_overlay_detail == MINIMUM ? 82.5f : 140.f) * SCALE.x, (emuenv.cfg.performance_overlay_detail <= LOW ? 40.f : 63.f) * SCALE.y);

    ImGui::SetNextWindowSize(MAIN_WINDOW_SIZE);
    ImGui::SetNextWindowPos(WINDOW_POS);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::Begin("##performance", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, PERF_OVERLAY_BG_COLOR);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.f * SCALE.x);
    ImGui::BeginChild("#perf_stats", WINDOW_SIZE, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings);
    ImGui::PushFont(gui.vita_font);
    ImGui::SetWindowFontScale(0.7f * RES_SCALE.x);
    if (emuenv.cfg.performance_overlay_detail == PerfomanceOverleyDetail::MINIMUM)
        ImGui::Text("FPS: %d", emuenv.fps);
    else
        ImGui::Text("FPS: %d %s: %d", emuenv.fps, lang["avg"].c_str(), emuenv.avg_fps);
    if (emuenv.cfg.performance_overlay_detail >= PerfomanceOverleyDetail::MEDIUM) {
        ImGui::Separator();
        ImGui::Text("%s: %d %s: %d", lang["min"].c_str(), emuenv.min_fps, lang["max"].c_str(), emuenv.max_fps);
    }
    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    if (emuenv.cfg.performance_overlay_detail == PerfomanceOverleyDetail::MAXIMUM) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (3.f * SCALE.y));
        ImGui::PlotLines("##fps_graphic", emuenv.fps_values, IM_ARRAYSIZE(emuenv.fps_values), emuenv.current_fps_offset, nullptr, 0.f, float(emuenv.max_fps), WINDOW_SIZE);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

} // namespace gui
