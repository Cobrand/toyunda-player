// Build with: gcc -o main main.c `pkg-config --libs --cflags mpv sdl2`

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <memory>
#include <utility>
#include <functional>

#include <docopt.h>

#include "mpv_wrapper.hpp"
#include "sdl_wrapper.hpp"

extern "C" {
#include <SDL2/SDL.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
}

extern "C" {
static Uint32 wakeup_on_mpv_redraw, wakeup_on_mpv_events;

static void die( const char *msg )
{
    fprintf( stderr, "%s\n", msg );
    exit( 1 );
}

static void *get_proc_address_mpv( void *, const char *name )
{
    return SDL_GL_GetProcAddress( name );
}

static void on_mpv_events( void * )
{
    SDL_Event event = {.type = wakeup_on_mpv_events};
    SDL_PushEvent( &event );
}

static void on_mpv_redraw( void * )
{
    SDL_Event event = {.type = wakeup_on_mpv_redraw};
    SDL_PushEvent( &event );
}
}

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
        die( "SDL init failed" );
    }
    SDL::Window_ptr window{SDL_CreateWindow(
        "Toyunda Player",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1000,
        500,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE )};
    if( !window ) {
        die( "failed to create SDL window" );
    }

    MPV::openGL_CB_context mpv_gl{mpv, MPV::sub_api::OPENGL_CB};

    SDL_GLContext glcontext = SDL_GL_CreateContext( window.get() );
    if( !glcontext ) {
        die( "failed to create SDL GL context" );
    }

    MPV::opengl_cb_init_gl( mpv_gl, get_proc_address_mpv );

    MPV::set_option_string( mpv, "vo", "opengl-cb" );

    // We use events for thread-safe notification of the SDL main loop.
    // Generally, the wakeup callbacks (set further below) should do as least
    // work as possible, and merely wake up another thread to do actual work.
    // On SDL, waking up the mainloop is the ideal course of action. SDL's
    // SDL_PushEvent() is thread-safe, so we use that.
    wakeup_on_mpv_redraw = SDL_RegisterEvents( 1 );
    wakeup_on_mpv_events = SDL_RegisterEvents( 1 );
    if( wakeup_on_mpv_redraw == (Uint32)-1 ||
        wakeup_on_mpv_events == (Uint32)-1 ) {
        die( "could not register events" );
    }

    // When normal mpv events are available.
    mpv_set_wakeup_callback( mpv.get(), on_mpv_events, NULL );

    // When a new frame should be drawn with mpv_opengl_cb_draw().
    // (Separate from the normal event handling mechanism for the sake of
    //  users which run OpenGL on a different thread.)
    mpv_opengl_cb_set_update_callback( mpv_gl.get(), on_mpv_redraw, NULL );

    // Play this file. Note that this starts playback asynchronously.
    const char *cmd[] = {"loadfile", mediaFile.c_str(), NULL};
    mpv_command( mpv.get(), cmd );
    double speed = 1.0;
    mpv_set_property( mpv.get(), "speed", MPV_FORMAT_DOUBLE, &speed );
    bool finished = false;
    SDL::Event_Dispatcher handler =
        SDL::build( mpv, wakeup_on_mpv_redraw, wakeup_on_mpv_events, speed );
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
