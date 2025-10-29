/**
 * \file DKSpriteSheet.cpp
 * \brief Sprite sheet implementation
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#include "DKSpriteSheet.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Engine/Path.hpp"
namespace Dokev
{
    SpriteSheet::SpriteSheet(const std::filesystem::path& spt_file)
    {
        std::ifstream file(assets::locate_asset(spt_file));
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open .spt file: " + spt_file.string());
        }

        std::string line;
        std::string texturePath;
        bool texturePathFound = false;
        
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty())
                continue;

            std::istringstream iss(line);
            std::string command;
            iss >> command;

            if (!texturePathFound &&
                (command.find(".png") != std::string::npos ||
                 command.find(".jpg") != std::string::npos ||
                 command.find(".jpeg") != std::string::npos ||
                 command.find("Assets/") != std::string::npos))
            {
                size_t commentPos = command.find('#');
                if (commentPos != std::string::npos)
                {
                    texturePath = command.substr(0, commentPos);
                }
                else
                {
                    texturePath = command;
                }
                texturePath.erase(texturePath.find_last_not_of(" \t\r\n") + 1);
                texturePathFound = true;
            }
            else if (command == "HotSpot")
            {
                iss >> hotSpot.x >> hotSpot.y;
            }
            else if (command == "FrameSize")
            {
                iss >> frameSize.x >> frameSize.y;
            }
            else if (command == "Frame")
            {
                Math::ivec2 pos;
                iss >> pos.x >> pos.y;
                framePositions.push_back(pos);
            }
            else if (command == "Anim")
            {
                iss >> animationFile;
            }
        }

        if (!texturePath.empty())
        {
            texture = Dokev::LoadTexture(texturePath);

            if (!texture.IsValid())
            {
                throw std::runtime_error("Failed to load texture: " + texturePath);
            }
        }
        else
        {
            throw std::runtime_error("No texture path found in .spt file: " + spt_file.string());
        }

        if (framePositions.empty())
        {
            throw std::runtime_error("No frames defined in .spt file: " + spt_file.string());
        }

        if (hotSpot.x == 0 && hotSpot.y == 0)
        {
            hotSpot.x = frameSize.x / 2.0f;
            hotSpot.y = frameSize.y / 2.0f;
        }
    }

    SpriteSheet::SpriteSheet(Texture2D texture, 
                             Math::ivec2 frameSize,
                             std::vector<Math::ivec2> framePositions,
                             Math::vec2 hotSpot)
        : texture(texture)
        , frameSize(frameSize)
        , framePositions(std::move(framePositions))
        , hotSpot(hotSpot)
    {
        if (this->hotSpot.x == 0 && this->hotSpot.y == 0)
        {
            this->hotSpot.x = frameSize.x / 2.0f;
            this->hotSpot.y = frameSize.y / 2.0f;
        }
    }

    Math::ivec2 SpriteSheet::GetFramePosition(int frameIndex) const
    {
        if (frameIndex < 0 || frameIndex >= static_cast<int>(framePositions.size()))
        {
            return { 0, 0 };
        }
        return framePositions[frameIndex];
    }

    Math::vec4 SpriteSheet::GetFrameUV(int frameIndex) const
    {
        if (frameIndex < 0 || frameIndex >= static_cast<int>(framePositions.size()))
        {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        }

        if (!texture.IsValid())
        {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        }

        Math::ivec2 texSize = texture.GetSize();
        
        if (texSize.x == 0 || texSize.y == 0)
        {
            return { 0.0f, 0.0f, 1.0f, 1.0f };
        }

        Math::ivec2 pos = framePositions[frameIndex];

        float left   = static_cast<float>(pos.x) / static_cast<float>(texSize.x);
        float right  = static_cast<float>(pos.x + frameSize.x) / static_cast<float>(texSize.x);

        float top    = 1.0f - (static_cast<float>(pos.y) / static_cast<float>(texSize.y));
        float bottom = 1.0f - (static_cast<float>(pos.y + frameSize.y) / static_cast<float>(texSize.y));


        return { left, bottom, right, top };
    }
}