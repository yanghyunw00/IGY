/**
 * \file GameConsole.cpp
 */
#include "GameConsole.hpp"

#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Engine/Engine.hpp"
#include "Engine/FontManager.hpp"
#include "Engine/Window.hpp"

#include <SDL.h>
#include <sstream>

// helpers 

namespace
{
    bool JustPressed(bool current, bool& prev)
    {
        bool fired = current && !prev;
        prev       = current;
        return fired;
    }

    std::vector<std::string> Tokenize(const std::string& line)
    {
        std::vector<std::string> tokens;
        std::istringstream       ss(line);
        std::string              tok;
        while (ss >> tok)
            tokens.push_back(tok);
        return tokens;
    }
}

//  Init 

void GameConsole::Init(const std::string& name)
{
    fontName = name;

    auto font = CS230::FontManager::Instance().Get(fontName);
    if (font)
    {
        // Measure a typical character to get the natural line height, then scale it.
        lineHeight = static_cast<float>(font->measureText("A").y) * fontScale + 4.f;
    }

    // Built-in commands
    RegisterCommand("help", "List all commands", [this](const auto&)
    {
        std::string out;
        for (const auto& [n, c] : commands)
            out += "  " + n + " — " + c.desc + "\n";
        return out;
    });

    RegisterCommand("clear", "Clear the console", [this](const auto&)
    {
        items.clear();
        return std::string{};
    });

    AddLog("Console ready. Press ` to toggle, Esc to close.", Dokev::ColorPalette::LIGHTGRAY);
    AddLog("Type 'help' for available commands.", Dokev::ColorPalette::LIGHTGRAY);
}

// ── RegisterCommand / AddLog ─────────────────────────────────────────────────

void GameConsole::RegisterCommand(const std::string& name, const std::string& desc, CommandFn fn)
{
    commands[name] = { desc, std::move(fn) };
}

void GameConsole::AddLog(const std::string& text, CS200::RGBA color)
{
    // Split multi-line strings into separate entries
    std::istringstream ss(text);
    std::string        line;
    while (std::getline(ss, line))
    {
        if (!line.empty())
            items.push_back({ line, color });
    }

    if (static_cast<int>(items.size()) > kMaxLogLines)
        items.erase(items.begin(), items.begin() + static_cast<int>(items.size()) - kMaxLogLines);
}

// ── ExecuteCommand ───────────────────────────────────────────────────────────

void GameConsole::ExecuteCommand(const std::string& cmdLine)
{
    if (cmdLine.empty())
        return;

    history.push_back(cmdLine);
    historyPos = -1;

    AddLog("> " + cmdLine, Dokev::ColorPalette::GOLD);

    auto tokens = Tokenize(cmdLine);
    if (tokens.empty())
        return;

    auto it = commands.find(tokens[0]);
    if (it == commands.end())
    {
        AddLog("Unknown command: '" + tokens[0] + "'. Type 'help'.", Dokev::ColorPalette::RED);
        return;
    }

    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    std::string              result = it->second.fn(args);
    if (!result.empty())
        AddLog(result, CS200::WHITE);
}

// ── Update ───────────────────────────────────────────────────────────────────

void GameConsole::Update()
{
    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    // Toggle open/close with backtick (`) or Tab
    bool togglePressed = JustPressed(keys[SDL_SCANCODE_GRAVE] != 0, prevGrave) ||
                         JustPressed(keys[SDL_SCANCODE_TAB]   != 0, prevTab);
    if (togglePressed)
    {
        isOpen = !isOpen;
        if (isOpen)
            Engine::GetWindow().EnableTextInput();
        else
            Engine::GetWindow().DisableTextInput();
        return;
    }

    if (!isOpen)
        return;

    // Close with Escape
    if (JustPressed(keys[SDL_SCANCODE_ESCAPE] != 0, prevEscape))
    {
        isOpen = false;
        Engine::GetWindow().DisableTextInput();
        inputLine.clear();
        return;
    }

    const std::string& text = Engine::GetWindow().GetTextInput();
    for (char c : text)
    {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (uc >= 32 && uc <= 126 && static_cast<int>(inputLine.size()) < 200)
            inputLine += c;
    }

    // Backspace
    if (JustPressed(keys[SDL_SCANCODE_BACKSPACE] != 0, prevBackspace))
    {
        if (!inputLine.empty())
            inputLine.pop_back();
    }

    // Enter — execute
    if (JustPressed(keys[SDL_SCANCODE_RETURN] != 0, prevEnter))
    {
        ExecuteCommand(inputLine);
        inputLine.clear();
    }

    // Up — history older
    if (JustPressed(keys[SDL_SCANCODE_UP] != 0, prevUp))
    {
        if (!history.empty())
        {
            if (historyPos == -1)
                historyPos = static_cast<int>(history.size()) - 1;
            else if (historyPos > 0)
                --historyPos;
            inputLine = history[historyPos];
        }
    }

    // Down — history newer
    if (JustPressed(keys[SDL_SCANCODE_DOWN] != 0, prevDown))
    {
        if (historyPos != -1)
        {
            ++historyPos;
            if (historyPos >= static_cast<int>(history.size()))
            {
                historyPos = -1;
                inputLine.clear();
            }
            else
            {
                inputLine = history[historyPos];
            }
        }
    }
}

// ── Draw ─────────────────────────────────────────────────────────────────────

void GameConsole::Draw() const
{
    if (!isOpen)
        return;

    auto windowSize = Engine::GetWindow().GetSize();
    float H         = static_cast<float>(windowSize.y);
    float consoleH     = H * consoleHeightF;
    float consoleBaseY = H - consoleH; // y of the bottom edge of the console

    // Background
    Dokev::DrawRectangle(0, static_cast<int>(consoleBaseY), windowSize.x, static_cast<int>(consoleH), 0x000000CC);

    // Separator line between log and input
    float separatorY = consoleBaseY + lineHeight + 6.f;
    Dokev::DrawRectangle(0, static_cast<int>(separatorY), windowSize.x, 2, Dokev::ColorPalette::GRAY);

    // Input line
    std::string prompt = "> " + inputLine + "_";
    CS230::FontManager::Instance().DrawText(
        fontName, prompt,
        { 8.f, consoleBaseY + 3.f },
        CS230::TextAlign::Left,
        CS200::WHITE,
        { fontScale, fontScale });

    // Log lines — fill from separator up to top of console
    const float logAreaTop  = H;                // top of console = top of screen
    const float logAreaBot  = separatorY + 4.f; // just above separator
    int         maxVisible  = static_cast<int>((logAreaTop - logAreaBot) / lineHeight);

    int startIdx = std::max(0, static_cast<int>(items.size()) - maxVisible);
    for (int i = startIdx; i < static_cast<int>(items.size()); ++i)
    {
        float lineY = logAreaTop - lineHeight * static_cast<float>(i - startIdx + 1);
        CS230::FontManager::Instance().DrawText(
            fontName, items[i].text,
            { 8.f, lineY },
            CS230::TextAlign::Left,
            items[i].color,
            { fontScale, fontScale });
    }
}
