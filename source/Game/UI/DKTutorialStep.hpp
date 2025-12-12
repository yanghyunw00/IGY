/**
 * \file DKTutorialStep.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Engine/Vec2.hpp"
#include <string>


namespace Dokev
{
    // Completion condition type
    enum class CompletionType
    {
        CLICK_ANYWHERE,  // Click anywhere to proceed
        TIME_PASSED,     // Auto proceed after duration
        BUTTON_CLICKED,  // Click specific button
        BUILDING_PLACED, // Place a building
        NONE             // No condition (auto proceed)
    };

    // Highlight type
    enum class HighlightType
    {
        NONE,       // No highlight
        CIRCLE,     // Circle highlight
        RECTANGLE,  // Rectangle highlight
        FULL_SCREEN // Full screen
    };

    class TutorialStep
    {
    public:
        std::string text;          // Tutorial text
        Math::fvec2 highlightPos;  // Position of highlight area
        Math::fvec2 highlightSize; // Size of highlight area
        Math::fvec2 arrowPos;      // Arrow position

        CompletionType completionType; // Completion condition type
        HighlightType  highlightType;  // Highlight type
        float          duration;       // Duration for TIME_PASSED type

        bool showArrow;  // Show arrow indicator
        bool blockInput; // Block user input

        TutorialStep();

        // Check if step is completed
        bool IsCompleted(float elapsedTime) const;
    };
}
