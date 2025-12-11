/**
 * \file DKTutorialStep.cpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "DKTutorialStep.hpp"

namespace Dokev
{
    TutorialStep::TutorialStep()
        : text("")
        , highlightPos(0.0f, 0.0f)
        , highlightSize(0.0f, 0.0f)
        , arrowPos(0.0f, 0.0f)
        , completionType(CompletionType::CLICK_ANYWHERE)
        , highlightType(HighlightType::NONE)
        , duration(0.0f)
        , showArrow(false)
        , blockInput(false)
    {
    }

    bool TutorialStep::IsCompleted(float elapsedTime) const
    {
        switch (completionType)
        {
            case CompletionType::TIME_PASSED:
                return elapsedTime >= duration;

            case CompletionType::NONE:
                return true;

            case CompletionType::CLICK_ANYWHERE:
            case CompletionType::BUTTON_CLICKED:
            case CompletionType::BUILDING_PLACED:
                return false;

            default:
                return false;
        }
    }
}
