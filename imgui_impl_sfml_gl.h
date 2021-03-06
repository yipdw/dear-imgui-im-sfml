#ifndef DEAR_IMGUI_IM_SFML_IMGUI_IMPL_SFML_GL_H
#define DEAR_IMGUI_IM_SFML_IMGUI_IMPL_SFML_GL_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

bool ImGui_ImplSfmlGL_Init();
void ImGui_ImplSfmlGL_Shutdown();
void ImGui_ImplSfmlGL_NewFrame( sf::RenderTarget& target, const sf::Time& dt );
bool ImGui_ImplSfmlGL_ProcessEvent( const sf::Event& event );

void ImGui_ImplSfmlGL_InvalidateDeviceObjects();
bool ImGui_ImplSfmlGL_CreateDeviceObjects();

#endif //DEAR_IMGUI_IM_SFML_IMGUI_IMPL_SFML_GL_H
