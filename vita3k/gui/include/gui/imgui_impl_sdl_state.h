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

#pragma once

#include <imgui.h>

#include <cstdint>

struct SDL_Window;
struct SDL_Cursor;

namespace renderer {
struct State;
}

struct ImGui_State {
    SDL_Window *window{};
    renderer::State *renderer{};

    uint64_t time;
    int MouseButtonsDown;
    SDL_Cursor *MouseCursors[ImGuiMouseCursor_COUNT];
    int PendingMouseLeaveFrame;
    bool MouseCanUseGlobalState;

    bool init;
    bool is_typing;
    bool do_clear_screen;

    ImGui_State() {
        memset((void *)this, 0, sizeof(*this));
        do_clear_screen = true;
    }
    virtual ~ImGui_State() = default;
};

class ImGui_Texture {
    ImGui_State *state = nullptr;
    ImTextureID texture_id = nullptr;

public:
    ImGui_Texture() = default;
    ImGui_Texture(ImGui_State *new_state, void *data, int width, int height);
    ImGui_Texture(ImGui_Texture &&texture) noexcept;

    void init(ImGui_State *new_state, ImTextureID texture);
    void init(ImGui_State *new_state, void *data, int width, int height);

    operator bool() const;
    operator ImTextureID() const;
    bool operator==(const ImGui_Texture &texture);

    ImGui_Texture &operator=(ImGui_Texture &&texture) noexcept;
    ImGui_Texture &operator=(const ImGui_Texture &texture) = delete;

    ~ImGui_Texture();
};
