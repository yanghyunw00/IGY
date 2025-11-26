#pragma once
#include <string>
#include "Engine/Vec2.hpp"

namespace Dokev
{
    // Completion condition type
    enum class CompletionType
    {
        CLICK_ANYWHERE,      // Click anywhere to proceed
        TIME_PASSED,         // Auto proceed after duration
        BUTTON_CLICKED,      // Click specific button
        BUILDING_PLACED,     // Place a building
        NONE                 // No condition (auto proceed)
    };

    // Highlight type
    enum class HighlightType
    {
        NONE,               // No highlight
        CIRCLE,             // Circle highlight
        RECTANGLE,          // Rectangle highlight
        FULL_SCREEN         // Full screen
    };

    class TutorialStep
    {
    public:
        std::string text;                    // Tutorial text
        Math::fvec2 highlightPos;            // Position of highlight area
        Math::fvec2 highlightSize;           // Size of highlight area
        Math::fvec2 arrowPos;                // Arrow position

        CompletionType completionType;       // Completion condition type
        HighlightType highlightType;         // Highlight type
        float duration;                      // Duration for TIME_PASSED type

        bool showArrow;                      // Show arrow indicator
        bool blockInput;                     // Block user input

        TutorialStep();

        // Check if step is completed
        bool IsCompleted(float elapsedTime) const;
    };
}


/*
튜토리얼 흐름정의
항상 말풍선 모양으로 하단에 떠야하고 스킵창(F)이 떠야함!
1단계 : 어서오세요 이 게임은 시티빌딩 게임입니다 자신만의 멋진 도시를 짓고 주민들을 모아보세ㅐ요!
2단계 : 우선 자원을 설명드릴게요 -> 현재 가지고 있는 자원과 인간의 수를 확인 할 수 있습니다
애니메이션: 우측 자원ui창 동그랗게 하이라이트로 강조( 3초간 보여줌)
3단계 : 이제 건물을 지어볼게요!
애니메이션 : 건물 버튼 강조 + 클릭 유도
4단계 : 원하는 곳을 클릭하세요! 
애니메이션 : 맵 강조
5단계 : 건물이 지어졌습니다!
6단계 : 각 건물은 특정 자원을 생산할 수 있습니다! 
7단계 : 시민들을 위해 집과  도로를 건설하여 시민을 모으고 
나만의 도시를 만들어보세요! 
*/