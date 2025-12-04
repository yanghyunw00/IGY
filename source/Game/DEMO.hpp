/**
 * \file GamePlayState.hpp
 * \author junseok lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "CS200/RGBA.hpp"
#include "Engine/Matrix.hpp"
#include "Dokev/DKTexture.hpp"
#include "Dokev/DKCamera.hpp"
#include "Dokev/DKMouseInput.hpp"
#include "Dokev/DKGameState.hpp"
#include "CS200/IRenderer2D.hpp"

#include "Dokev/DKRenderState.hpp"

#include "Engine/Vec2.hpp"
#include "GridMap.hpp"
#include <array>
#include <memory>
#include <vector>


class DEMO : public Dokev::GameState
{
public:
    void          Load() override;
    void          Update(double dt) override;
    void          Unload() override;
    void          Draw() const override;
    void          SwitchRenderer(Dokev::RenderState type);
    void          DrawImGui() override;
    gsl::czstring GetName() const override;

private:
    Dokev::Texture2D testTexture;
    Dokev::Texture2D noiseTexture;
    Dokev::Texture2D noiseTexture2;

    GridMap map;

    //Iso grid
    int index = 1;

    Dokev::Camera camera;
    Dokev::Mouse mouse;
    Dokev::RenderState selected_renderer = Dokev::RenderState::BATCH; // default to batched renderer
private:
    void                       updateInput();
private: //test shi

    bool show_atlas = false;

    // Batch renderer showcase
    std::vector<Math::vec2> batchDemoPositions{};
    bool                    batchDemoEnabled = true;
};
