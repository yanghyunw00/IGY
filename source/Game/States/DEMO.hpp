/**
 * \file GamePlayState.hpp
 * \author junseok lee
 * \author
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "CS200/RGBA.hpp"
#include "Dokev/State/DKGameState.hpp"
#include "Engine/Matrix.hpp"
#include "Dokev/Graphics/DKTexture.hpp"
#include "Dokev/Graphics/DKCamera.hpp"
#include "Dokev/Input/DKMouseInput.hpp"
#include "CS200/IRenderer2D.hpp"

#include "Dokev/Graphics/DKRenderState.hpp"

#include "Engine/Vec2.hpp"
#include "../World/GridMap.hpp"
#include <array>
#include <memory>


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
    void          SetMapVariant(MapVariant variant) { mapVariant = variant; }

protected:
    Dokev::Texture2D testTexture;
    Dokev::Texture2D noiseTexture;
    Dokev::Texture2D noiseTexture2;

    GridMap map;
    MapVariant mapVariant = MapVariant::Balanced;

    //Iso grid
    int index = 1;

    Dokev::Camera camera;
    Dokev::Mouse mouse;
    std::unique_ptr<CS200::IRenderer2D>          customRenderer;
    Dokev::RenderState selected_renderer = Dokev::RenderState::INSTANCED; // will change to batched
private:
    void                       updateInput();
private: //test shi

    bool show_atlas = false;
};
