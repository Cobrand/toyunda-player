// Build with: gcc -o main main.c `pkg-config --libs --cflags mpv sdl2`

#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <functional>

#include <docopt.h>

#include "mpv_wrapper.hpp"
#include "sdl_wrapper.hpp"

#include <SDL2/SDL.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>


static Uint32 wakeup_on_mpv_redraw, wakeup_on_mpv_events;
/*
void on_mpv_events( void * )
{
    SDL_Event event ;
    event.type = wakeup_on_mpv_events;
    SDL_PushEvent( &event );
}

void on_mpv_redraw( void * )
{
    SDL_Event event ;
    event.type = wakeup_on_mpv_redraw;
    SDL_PushEvent( &event );
}
*/
int main( int argc, char *argv[] )
{
    constexpr char USAGE[]{
        R"(toyunda-player.
     Usage:
         toyounda-player [options] <file>

     Options:
         --invert  invert the screen
    )"};

    std::map< std::string, docopt::value > args{docopt::docopt(
        USAGE, {argv + 1, argv + argc}, true, "toyounda-player" )};
    auto const mediaFile = args["<file>"].asString();
    const int factor     = args["--invert"].asBool() ? 1 : -1;

    MPV::Handle_ptr mpv{};

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        throw std::runtime_error("failed to init SDL");
    }
    SDL::Window_ptr window{SDL_CreateWindow(
        "Toyunda Player",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000,
        500,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE )};
    if( !window ) {
        throw std::runtime_error("failed to create SDL window");
    }

    MPV::openGL_CB_context mpv_gl{mpv, MPV::sub_api::OPENGL_CB};

    SDL_GLContext glcontext = SDL_GL_CreateContext( window.get() );
    if( !glcontext ) {
        throw std::runtime_error("failed to create GL context");
    }

    MPV::opengl_cb_init_gl( mpv_gl, [](void *, const char *name){
        return SDL_GL_GetProcAddress( name );
    });

    MPV::set_option_string( mpv, "vo", "opengl-cb" );


    wakeup_on_mpv_events = SDL_RegisterEvents( 1 );
    mpv_set_wakeup_callback( mpv.get(), [](void*) {
        SDL_Event event ;
        event.type = wakeup_on_mpv_events;
        SDL_PushEvent( &event );
    }, NULL );

    wakeup_on_mpv_redraw = SDL_RegisterEvents( 1 );
    mpv_opengl_cb_set_update_callback( mpv_gl.get(), [](void*) {
        SDL_Event event ;
        event.type = wakeup_on_mpv_redraw;
        SDL_PushEvent( &event );
    }, NULL );


    // Play this file. Note that this starts playback asynchronously.
    const char *cmd[] = {"loadfile", mediaFile.c_str(), NULL};
    mpv_command( mpv.get(), cmd );
    double speed = 1.0;
    mpv_set_property( mpv.get(), "speed", MPV_FORMAT_DOUBLE, &speed );
    bool finished                 = false;
    SDL::Event_Dispatcher handler = SDL::build( mpv,
                                                wakeup_on_mpv_redraw,
                                                wakeup_on_mpv_events,
                                                speed );
    while( !finished ) {
        SDL_Event event;
        if( SDL_WaitEvent( &event ) != 1 ) {
            throw std::runtime_error( "event loop error" );
        }

        switch( handler.handle( event ) ) {
            case SDL::Event_Dispatcher::Result::finished:
                finished = true;
                break;
            case SDL::Event_Dispatcher::Result::redraw:
                SDL::redraw( window, mpv_gl, factor );
                break;
            default: break;
        }
    }
    SDL_Quit();
    return 0;
}
