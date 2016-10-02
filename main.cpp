#include <SFML/Window.hpp>
#include <imgui.h>
#include "imgui_impl_sfml_gl.h"
#include "gl_imgui_gl_core_3_2.hpp"

int main()
{
    sf::ContextSettings context_settings;
    context_settings.depthBits = 24;
    context_settings.stencilBits = 8;
    context_settings.antialiasingLevel = 2;
    context_settings.majorVersion = 3;
    context_settings.minorVersion = 2;
    context_settings.attributeFlags = sf::ContextSettings::Attribute::Core;

    sf::Window window{ sf::VideoMode{ 1280, 720 }, "Dear imgui,", sf::Style::Default, context_settings };

    if ( !ImGui_ImplSfmlGL_Init() ) {
        std::cerr << "unable to initialize imgui" << std::endl;
        return 1;
    }

    bool running = true;
    bool show_test_window = false;

    window.setActive( true );
    window.setVisible( true );
    window.setVerticalSyncEnabled( true );

    sf::Clock clock;

    while ( running ) {
        sf::Event event;

        while ( window.pollEvent( event ) ) {
            using e = sf::Event;

            switch( event.type ) {
                case e::Closed:
                    running = false;
                    break;
                default:
                    break;
            }

            ImGui_ImplSfmlGL_ProcessEvent( event );
        }

        const sf::Time elapsed = clock.restart();
        ImGui_ImplSfmlGL_NewFrame( window, elapsed );
        ImGui::ShowTestWindow( &show_test_window );

        imgui::gl::Clear(imgui::gl::COLOR_BUFFER_BIT);
        ImGui::Render();
        window.display();
    }

    ImGui_ImplSfmlGL_Shutdown();
    return 0;
}
