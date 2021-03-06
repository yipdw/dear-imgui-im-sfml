#include <SFML/System.hpp>
#include <GL/glew.h>
#include <imgui.h>
#include "imgui_impl_sfml_gl.h"

// Data
static GLuint g_FontTexture = 0;
static bool g_MousePressed[5] = { false, false, false, false, false };
static float g_MouseWheelDelta = 0.0f;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplSfmlGL_RenderDrawLists( ImDrawData* draw_data )
{
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = ( int ) ( io.DisplaySize.x * io.DisplayFramebufferScale.x );
    int fb_height = ( int ) ( io.DisplaySize.y * io.DisplayFramebufferScale.y );
    if ( fb_width == 0 || fb_height == 0 )
        return;
    draw_data->ScaleClipRects( io.DisplayFramebufferScale );

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
    GLint last_texture;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
    GLint last_viewport[4];
    glGetIntegerv( GL_VIEWPORT, last_viewport );
    glPushAttrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_SCISSOR_TEST );
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnable( GL_TEXTURE_2D );
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

    // Setup viewport, orthographic projection matrix
    glViewport( 0, 0, ( GLsizei ) fb_width, ( GLsizei ) fb_height );
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    glOrtho( 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f );
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
#define OFFSETOF( TYPE, ELEMENT ) ((size_t)&(((TYPE *)0)->ELEMENT))
    for ( int n = 0; n < draw_data->CmdListsCount; n++ ) {
        const ImDrawList* cmd_list = draw_data->CmdLists[ n ];
        const unsigned char* vtx_buffer = ( const unsigned char* ) &cmd_list->VtxBuffer.front();
        const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
        glVertexPointer( 2, GL_FLOAT, sizeof( ImDrawVert ), ( void* ) ( vtx_buffer + OFFSETOF( ImDrawVert, pos ) ) );
        glTexCoordPointer( 2, GL_FLOAT, sizeof( ImDrawVert ), ( void* ) ( vtx_buffer + OFFSETOF( ImDrawVert, uv ) ) );
        glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( ImDrawVert ),
                        ( void* ) ( vtx_buffer + OFFSETOF( ImDrawVert, col ) ) );

        for ( int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++ ) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[ cmd_i ];
            if ( pcmd->UserCallback ) {
                pcmd->UserCallback( cmd_list, pcmd );
            }
            else {
                glBindTexture( GL_TEXTURE_2D, ( GLuint ) ( intptr_t ) pcmd->TextureId );
                glScissor( ( int ) pcmd->ClipRect.x, ( int ) ( fb_height - pcmd->ClipRect.w ),
                           ( int ) ( pcmd->ClipRect.z - pcmd->ClipRect.x ),
                           ( int ) ( pcmd->ClipRect.w - pcmd->ClipRect.y ) );
                glDrawElements( GL_TRIANGLES, ( GLsizei ) pcmd->ElemCount,
                                sizeof( ImDrawIdx ) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer );
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
#undef OFFSETOF

    // Restore modified state
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
    glBindTexture( GL_TEXTURE_2D, ( GLuint ) last_texture );
    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glPopAttrib();
    glViewport( last_viewport[ 0 ], last_viewport[ 1 ], ( GLsizei ) last_viewport[ 2 ],
                ( GLsizei ) last_viewport[ 3 ] );
}

bool ImGui_ImplSfmlGL_Init()
{
    ImGuiIO& io = ImGui::GetIO();

    io.KeyMap[ ImGuiKey_Tab ] = sf::Keyboard::Tab;
    io.KeyMap[ ImGuiKey_LeftArrow ] = sf::Keyboard::Left;
    io.KeyMap[ ImGuiKey_RightArrow ] = sf::Keyboard::Right;
    io.KeyMap[ ImGuiKey_UpArrow ] = sf::Keyboard::Up;
    io.KeyMap[ ImGuiKey_DownArrow ] = sf::Keyboard::Down;
    io.KeyMap[ ImGuiKey_PageUp ] = sf::Keyboard::PageUp;
    io.KeyMap[ ImGuiKey_PageDown ] = sf::Keyboard::PageDown;
    io.KeyMap[ ImGuiKey_Home ] = sf::Keyboard::Home;
    io.KeyMap[ ImGuiKey_End ] = sf::Keyboard::End;
    io.KeyMap[ ImGuiKey_Delete ] = sf::Keyboard::Delete;
    io.KeyMap[ ImGuiKey_Backspace ] = sf::Keyboard::BackSpace;
    io.KeyMap[ ImGuiKey_Enter ] = sf::Keyboard::Return;
    io.KeyMap[ ImGuiKey_Escape ] = sf::Keyboard::Escape;
    io.KeyMap[ ImGuiKey_A ] = sf::Keyboard::A;
    io.KeyMap[ ImGuiKey_C ] = sf::Keyboard::C;
    io.KeyMap[ ImGuiKey_V ] = sf::Keyboard::V;
    io.KeyMap[ ImGuiKey_X ] = sf::Keyboard::X;
    io.KeyMap[ ImGuiKey_Y ] = sf::Keyboard::Y;
    io.KeyMap[ ImGuiKey_Z ] = sf::Keyboard::Z;

    io.RenderDrawListsFn = ImGui_ImplSfmlGL_RenderDrawLists;

    return true;
}

void ImGui_ImplSfmlGL_Shutdown()
{
    ImGui_ImplSfmlGL_InvalidateDeviceObjects();
    ImGui::Shutdown();
}

void ImGui_ImplSfmlGL_NewFrame( sf::RenderTarget& target, const sf::Time& dt )
{
    if ( !g_FontTexture )
        ImGui_ImplSfmlGL_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    sf::Vector2u size = target.getSize();
    io.DisplaySize = ImVec2((float)size.x, (float)size.y);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.DeltaTime = dt.asSeconds();

    // Get mouse state (again)
    io.MouseDown[ 0 ] = g_MousePressed[ 0 ] || sf::Mouse::isButtonPressed( sf::Mouse::Button::Left );
    io.MouseDown[ 1 ] = g_MousePressed[ 1 ] || sf::Mouse::isButtonPressed( sf::Mouse::Button::Middle );
    io.MouseDown[ 2 ] = g_MousePressed[ 2 ] || sf::Mouse::isButtonPressed( sf::Mouse::Button::Right );
    io.MouseDown[ 3 ] = g_MousePressed[ 3 ] || sf::Mouse::isButtonPressed( sf::Mouse::Button::XButton1 );
    io.MouseDown[ 4 ] = g_MousePressed[ 4 ] || sf::Mouse::isButtonPressed( sf::Mouse::Button::XButton2 );
    io.MouseWheel     = g_MouseWheelDelta;
    g_MousePressed[ 0 ] = false;
    g_MousePressed[ 1 ] = false;
    g_MousePressed[ 2 ] = false;
    g_MousePressed[ 3 ] = false;
    g_MousePressed[ 4 ] = false;
    g_MouseWheelDelta   = 0.0f;

    ImGui::NewFrame();
}

bool ImGui_ImplSfmlGL_ProcessEvent( const sf::Event& event )
{
    ImGuiIO& io = ImGui::GetIO();

    switch ( event.type ) {
        case sf::Event::MouseMoved:
            io.MousePos = ImVec2( (float)event.mouseMove.x, (float)event.mouseMove.y );
            return true;
        case sf::Event::LostFocus:
            io.MousePos = ImVec2( -1.0f, -1.0f );
            return true;
        case sf::Event::MouseWheelScrolled:
            g_MouseWheelDelta = event.mouseWheelScroll.delta;
            return true;
        case sf::Event::MouseButtonPressed: {
            const auto& button = event.mouseButton.button;

            if ( button == sf::Mouse::Button::Left )
                g_MousePressed[ 0 ] = true;
            if ( button == sf::Mouse::Button::Middle )
                g_MousePressed[ 1 ] = true;
            if ( button == sf::Mouse::Button::Right )
                g_MousePressed[ 2 ] = true;
            if ( button == sf::Mouse::Button::XButton1 )
                g_MousePressed[ 3 ] = true;
            if ( button == sf::Mouse::Button::XButton2 )
                g_MousePressed[ 4 ] = true;

            return true;
        }
        case sf::Event::TextEntered: {
            char output[6] = { 0, 0, 0, 0, 0, 0 };
            sf::Utf8::encode( event.text.unicode, output );
            io.AddInputCharactersUTF8( output );
            return true;
        }
        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased:
            io.KeysDown[ event.key.code ] = event.type == sf::Event::KeyPressed;
            io.KeyShift = sf::Keyboard::isKeyPressed( sf::Keyboard::LShift ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RShift );
            io.KeyCtrl = sf::Keyboard::isKeyPressed( sf::Keyboard::LControl ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RControl );
            io.KeyAlt = sf::Keyboard::isKeyPressed( sf::Keyboard::LAlt ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RAlt );
            io.KeySuper = sf::Keyboard::isKeyPressed( sf::Keyboard::LSystem ) || sf::Keyboard::isKeyPressed( sf::Keyboard::RSystem );
            return true;
        default:
            return false;
    }
}

void ImGui_ImplSfmlGL_InvalidateDeviceObjects()
{
    if (g_FontTexture) {
        glDeleteTextures( 1, &g_FontTexture );
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}

bool ImGui_ImplSfmlGL_CreateDeviceObjects()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}
