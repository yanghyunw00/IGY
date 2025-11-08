// /**
//  * \file GamePlayState.hpp
//  * \author junseok lee
//  * \author
//  * \date 2025 Fall
//  * \par CS200 Computer Graphics I
//  * \copyright DigiPen Institute of Technology
//  */
// #include "CS200/RGBA.hpp"
// #include "Dokev/DKGameState.hpp"
// #include "Engine/Matrix.hpp"
// #include "Dokev/DKTexture.hpp"
// #include "Dokev/DKCamera.hpp"
// #include "Dokev/DKMouseInput.hpp"
// #include "CS200/IRenderer2D.hpp"

// #include "Dokev/DKRenderState.hpp"

// #include "Engine/Vec2.hpp"
// #include "GridMap.hpp"
// #include <array>
// #include <memory>


// class DEMO : public Dokev::GameState
// {
// public:
//     void          Load() override;
//     void          Update(double dt) override;
//     void          Unload() override;
//     void          Draw() const override;
//     void          SwitchRenderer(Dokev::RenderState type);
//     void          DrawImGui();
//     gsl::czstring GetName() const override;

// private:
//     Dokev::Texture2D testTexture;
//     Dokev::Texture2D noiseTexture;
//     Dokev::Texture2D noiseTexture2;

//     GridMap map;

//     //Iso grid
//     int index = 1;

//     Dokev::Camera camera;
//     Dokev::Mouse mouse;
//     std::unique_ptr<CS200::IRenderer2D>          customRenderer;
//     Dokev::RenderState selected_renderer = Dokev::RenderState::IMMEDIATE;
// private:
//     void                       updateInput();

// };