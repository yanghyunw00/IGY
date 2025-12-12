/**
 * \file DKTutorialManager.hpp
 * \brief
 * \author Hyunwoo Yang
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "DKTutorialStep.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Engine/Font.hpp"
#include <memory>
#include <vector>


namespace Dokev
{
    class UIManager;

    class TutorialManager
    {
    private:
        std::vector<TutorialStep> steps;            // All tutorial steps
        int                       currentStepIndex; // Current step index
        bool                      isActive;         // Is tutorial active
        float                     currentStepTime;  // Elapsed time for current step
        bool                      canSkip;          // Can skip tutorial

        std::unique_ptr<CS230::Font> font;     // Font for text
        Texture2D                    skipIcon; // Skip icon (F key)

        UIManager* uiManager; // UI Manager reference

    public:
        TutorialManager();

        void Initialize(UIManager* ui);                        // Create all tutorial steps
        void Update(float dt, bool clicked, bool skipPressed); // Update each frame
        void Draw() const;                                     // Draw tutorial UI
        void NextStep();                                       // Move to next step
        void Skip();                                           // Skip tutorial
        bool IsActive() const;                                 // Check if tutorial is active
        int  GetCurrentStep() const;                           // Get current step index

        // Event functions called from outside
        void OnButtonClicked();  // Button click event
        void OnBuildingPlaced(); // Building placed event

    private:
        void CreateSteps();            // Create 7 tutorial steps
        void DrawOverlay() const;      // Draw dark overlay
        void DrawHighlight() const;    // Draw highlight area
        void DrawSpeechBubble() const; // Draw speech bubble
        void DrawSkipHint() const;     // Draw skip hint (F key)
        void DrawArrow() const;        // Draw arrow indicator
    };
}