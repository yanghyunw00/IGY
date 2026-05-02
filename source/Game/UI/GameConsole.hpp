/**
 * \file GameConsole.hpp
 * \brief In-game console. No ImGui — uses the game's own font/shape renderer.
 *        Toggle with backtick (`). Close with Escape.
 */
#pragma once

#include "CS200/RGBA.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class GameConsole
{
public:
    using CommandFn = std::function<std::string(const std::vector<std::string>& args)>;

    // Call after the font is loaded (needs font to measure line height).
    void Init(const std::string& fontName);

    void RegisterCommand(const std::string& name, const std::string& desc, CommandFn fn);

    // Log a line. Default color = white.
    void AddLog(const std::string& text, CS200::RGBA color = 0xFFFFFFFF);

    void Update();
    void Draw() const;

    bool IsOpen() const { return isOpen; }

private:
    void ExecuteCommand(const std::string& cmdLine);

    struct LogEntry { std::string text; CS200::RGBA color; };
    struct CmdInfo  { std::string desc; CommandFn   fn;   };

    std::string                              fontName;
    std::vector<LogEntry>                    items;
    std::unordered_map<std::string, CmdInfo> commands;
    std::vector<std::string>                 history;
    int                                      historyPos   = -1;
    std::string                              inputLine;

    float lineHeight     = 20.f;
    float fontScale      = 0.6f;
    float consoleHeightF = 0.40f; // fraction of screen height

    bool isOpen = false;

    // Previous-frame key states for edge detection via SDL_GetKeyboardState
    bool prevGrave     = false;
    bool prevTab       = false;
    bool prevBackspace = false;
    bool prevEnter     = false;
    bool prevUp        = false;
    bool prevDown      = false;
    bool prevEscape    = false;

    static constexpr int kMaxLogLines = 200;
};
