/**
 * \file DKTutorialManager.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKTutorialManager.hpp"
#include "Dokev/Core/DKColor.hpp"
#include "Dokev/Graphics/DKShapes.hpp"
#include "Dokev/Managers/DKUIManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"
#include <cmath>
#include <sstream>


namespace Dokev
{
    TutorialManager::TutorialManager() : currentStepIndex(0), isActive(false), currentStepTime(0.0f), canSkip(true), uiManager(nullptr)
    {
    }

    void TutorialManager::Initialize(UIManager* ui)
    {
        uiManager        = ui;
        currentStepIndex = 0;
        isActive         = true;
        currentStepTime  = 0.0f;

        font = std::make_unique<CS230::Font>(assets::locate_asset("Assets/fonts/DungGeunMo.ttf"), 56);

        CreateSteps();
    }

    void TutorialManager::CreateSteps()
    {
        steps.clear();
        auto windowSize = Engine::GetWindow().GetSize();

        TutorialStep step1;
        step1.text           = "Welcome! Build your own city and gather citizens!";
        step1.completionType = CompletionType::CLICK_ANYWHERE;
        step1.highlightType  = HighlightType::NONE;
        step1.showArrow      = false;
        step1.blockInput     = false;
        steps.push_back(step1);

        TutorialStep step2;
        step2.text           = "Here you can check your money and population";
        step2.completionType = CompletionType::TIME_PASSED;
        step2.duration       = 2.0f;
        step2.highlightType  = HighlightType::CIRCLE;
        step2.highlightPos   = Math::fvec2(static_cast<float>(windowSize.x) - 150.0f, static_cast<float>(windowSize.y) - 200.0f);
        step2.highlightSize  = Math::fvec2(200.0f, 200.0f);
        step2.showArrow      = false;
        step2.blockInput     = true;
        steps.push_back(step2);

        TutorialStep step3;
        step3.text           = "Use WASD to move camera, Q/E to zoom in/out";
        step3.completionType = CompletionType::TIME_PASSED;
        step3.duration       = 3.0f;
        step3.highlightType  = HighlightType::NONE;
        step3.showArrow      = false;
        step3.blockInput     = false;
        steps.push_back(step3);

        float woodX = (static_cast<float>(windowSize.x) - 270.0f) / 2.0f + 280.0f;

        TutorialStep step4;
        step4.text           = "Now let's build a Wood Workshop to earn money!";
        step4.completionType = CompletionType::BUTTON_CLICKED;
        step4.highlightType  = HighlightType::RECTANGLE;
        step4.highlightPos   = Math::fvec2(woodX, 370.0f);
        step4.highlightSize  = Math::fvec2(60.0f, 60.0f);
        step4.showArrow      = true;
        step4.arrowPos       = Math::fvec2(woodX + 30.0f, 340.0f);
        step4.blockInput     = false;
        steps.push_back(step4);

        TutorialStep step5;
        step5.text           = "Buildings can only be placed next to roads!\nClick on the map to place your Wood Workshop!";
        step5.completionType = CompletionType::BUILDING_PLACED;
        step5.highlightType  = HighlightType::FULL_SCREEN;
        step5.showArrow      = false;
        step5.blockInput     = false;
        steps.push_back(step5);

        TutorialStep step6;
        step6.text           = "Building placed successfully!";
        step6.completionType = CompletionType::CLICK_ANYWHERE;
        step6.highlightType  = HighlightType::NONE;
        step6.showArrow      = false;
        step6.blockInput     = false;
        steps.push_back(step6);

        TutorialStep step7;
        step7.text           = "Staffed work buildings generate money over time!";
        step7.completionType = CompletionType::CLICK_ANYWHERE;
        step7.highlightType  = HighlightType::NONE;
        step7.showArrow      = false;
        step7.blockInput     = false;
        steps.push_back(step7);

        TutorialStep step8;
        step8.text           = "Build houses and roads to attract citizens.\nCreate your city!";
        step8.completionType = CompletionType::CLICK_ANYWHERE;
        step8.highlightType  = HighlightType::NONE;
        step8.showArrow      = false;
        step8.blockInput     = false;
        steps.push_back(step8);
    }

    void TutorialManager::Update(float dt, bool clicked, bool skipPressed)
    {
        if (!isActive)
            return;

        if (currentStepIndex >= static_cast<int>(steps.size()))
        {
            isActive = false;
            return;
        }

        if (skipPressed && canSkip)
        {
            Skip();
            return;
        }

        currentStepTime += dt;

        const auto& currentStep = steps[currentStepIndex];

        if (currentStep.IsCompleted(currentStepTime))
        {
            NextStep();
            return;
        }

        if (clicked && currentStep.completionType == CompletionType::CLICK_ANYWHERE)
        {
            NextStep();
        }
    }

    void TutorialManager::Draw() const
    {
        if (!isActive)
            return;

        if (currentStepIndex >= static_cast<int>(steps.size()))
            return;

        DrawOverlay();

        DrawHighlight();

        if (steps[currentStepIndex].showArrow)
        {
            DrawArrow();
        }

        DrawSpeechBubble();

        DrawSkipHint();
    }

    void TutorialManager::DrawOverlay() const
    {
        auto        windowSize  = Engine::GetWindow().GetSize();
        const auto& currentStep = steps[currentStepIndex];

        CS200::RGBA overlayColor = currentStep.highlightType == HighlightType::NONE ? ColorPalette::TUTORIAL_OVERLAY : ColorPalette::TUTORIAL_OVERLAY_DARK;

        DrawRectangle(0, 0, windowSize.x, windowSize.y, overlayColor);
    }

    void TutorialManager::DrawHighlight() const
    {
        const auto& currentStep = steps[currentStepIndex];

        if (currentStep.highlightType == HighlightType::NONE || currentStep.highlightType == HighlightType::FULL_SCREEN)
            return;

        int x      = static_cast<int>(currentStep.highlightPos.x);
        int y      = static_cast<int>(currentStep.highlightPos.y);
        int width  = static_cast<int>(currentStep.highlightSize.x);
        int height = static_cast<int>(currentStep.highlightSize.y);

        if (currentStep.highlightType == HighlightType::CIRCLE)
        {
            // float pulse = 1.0f + 0.1f * std::sin(currentStepTime * 3.0f);

            DrawRectangle(x, y, width, height, ColorPalette::TUTORIAL_HIGHLIGHT);
        }
        else if (currentStep.highlightType == HighlightType::RECTANGLE)
        {
            // Draw bright area
            DrawRectangle(x, y, width, height, ColorPalette::BLANK);
            DrawRectangleLinesEx(x, y, width, height, 3.0f, ColorPalette::YELLOW);
        }
    }

    void TutorialManager::DrawSpeechBubble() const
    {
        auto        windowSize  = Engine::GetWindow().GetSize();
        const auto& currentStep = steps[currentStepIndex];

        float bubbleWidth  = 900.0f;
        float bubbleHeight = 160.0f;
        float bubbleX      = (windowSize.x - bubbleWidth) / 2.0f;
        float bubbleY      = windowSize.y - bubbleHeight - 50.0f;

        DrawRectangle(static_cast<int>(bubbleX), static_cast<int>(bubbleY), static_cast<int>(bubbleWidth), static_cast<int>(bubbleHeight), ColorPalette::BLACK);

        DrawRectangleLinesEx(static_cast<int>(bubbleX), static_cast<int>(bubbleY), static_cast<int>(bubbleWidth), static_cast<int>(bubbleHeight), 4.0f, ColorPalette::WHITE);

        if (font)
        {
            const float scale       = 0.5f;
            const float lineHeight  = 32.0f;
            const float lineSpacing = 6.0f;
            const float cornerPad   = 10.0f;

            // Corner star decorators
            if (auto starTexture = font->PrintToTexture("*", ColorPalette::WHITE))
            {
                starTexture->Draw(Math::TranslationMatrix(Math::vec2{ bubbleX + cornerPad, bubbleY + cornerPad }) * Math::ScaleMatrix(Math::vec2{ scale, scale }), ColorPalette::WHITE);
                starTexture->Draw(Math::TranslationMatrix(Math::vec2{ bubbleX + bubbleWidth - lineHeight - cornerPad, bubbleY + bubbleHeight - lineHeight - cornerPad }) * Math::ScaleMatrix(Math::vec2{ scale, scale }), ColorPalette::WHITE);
            }

            // Split text by \n and render each line separately (TTF doesn't support newlines)
            std::vector<std::string> lines;
            std::istringstream       stream(currentStep.text);
            std::string              line;
            while (std::getline(stream, line))
                lines.push_back(line);

            float totalHeight = static_cast<float>(lines.size()) * lineHeight + static_cast<float>(lines.size() - 1) * lineSpacing;
            float startY      = bubbleY + (bubbleHeight - totalHeight) / 2.0f;

            for (size_t i = 0; i < lines.size(); ++i)
            {
                if (lines[i].empty())
                    continue;
                if (auto textTexture = font->PrintToTexture(lines[i], ColorPalette::WHITE))
                {
                    float lineY = startY + static_cast<float>(i) * (lineHeight + lineSpacing);
                    textTexture->Draw(Math::TranslationMatrix(Math::vec2{ bubbleX + 50.0f, lineY }) * Math::ScaleMatrix(Math::vec2{ scale, scale }), ColorPalette::WHITE);
                }
            }
        }
    }

    void TutorialManager::DrawSkipHint() const
    {
        if (!canSkip)
            return;

        auto windowSize = Engine::GetWindow().GetSize();

        float hintX = windowSize.x - 200.0f;
        float hintY = 20.0f;

        if (font)
        {
            std::string skipText = "Press F to Skip";
            if (auto textTexture = font->PrintToTexture(skipText, ColorPalette::YELLOW))
            {
                textTexture->Draw(Math::TranslationMatrix(Math::vec2{ hintX, hintY }) * Math::ScaleMatrix(Math::vec2{ 0.4, 0.4 }), ColorPalette::YELLOW);
            }
        }
    }

    void TutorialManager::DrawArrow() const
    {
        const auto& currentStep = steps[currentStepIndex];
        // Animated arrow
        float       offset      = 10.0f * std::sin(currentStepTime * 5.0f);
        int         arrowX      = static_cast<int>(currentStep.arrowPos.x);
        int         arrowY      = static_cast<int>(currentStep.arrowPos.y + offset);

        int arrowSize = 15;

        // Left line of arrow
        DrawLine(arrowX, arrowY, arrowX - arrowSize, arrowY - arrowSize, ColorPalette::TUTORIAL_ARROW);

        // Right line of arrow
        DrawLine(arrowX, arrowY, arrowX + arrowSize, arrowY - arrowSize, ColorPalette::TUTORIAL_ARROW);

        // Arrow shaft
        DrawLine(arrowX, arrowY - arrowSize, arrowX, arrowY - arrowSize * 2, ColorPalette::TUTORIAL_ARROW);
    }

    void TutorialManager::NextStep()
    {
        currentStepIndex++;
        currentStepTime = 0.0f;

        if (currentStepIndex >= static_cast<int>(steps.size()))
        {
            isActive = false;
        }

        // Step 4: Open Work category menu automatically
        if (currentStepIndex == 3 && uiManager)
        {
            uiManager->OpenBuildingCategory(CategoryType::WORK_CATEGORY);
        }
    }

    void TutorialManager::Skip()
    {
        isActive         = false;
        currentStepIndex = static_cast<int>(steps.size());
    }

    bool TutorialManager::IsActive() const
    {
        return isActive;
    }

    int TutorialManager::GetCurrentStep() const
    {
        return currentStepIndex;
    }

    void TutorialManager::OnButtonClicked()
    {
        if (!isActive)
            return;

        if (currentStepIndex >= static_cast<int>(steps.size()))
            return;

        const auto& currentStep = steps[currentStepIndex];
        if (currentStep.completionType == CompletionType::BUTTON_CLICKED)
        {
            NextStep();
        }
    }

    void TutorialManager::OnBuildingPlaced()
    {
        if (!isActive)
            return;

        if (currentStepIndex >= static_cast<int>(steps.size()))
            return;

        const auto& currentStep = steps[currentStepIndex];
        if (currentStep.completionType == CompletionType::BUILDING_PLACED)
        {
            NextStep();
        }
    }
}
