// Build with: gcc -o main main.c `pkg-config --libs --cflags mpv sdl2`

#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <functional>

#include <docopt.h>
/*
#include "mpv_wrapper.hpp"
#include "sdl_wrapper.hpp"
*/
#include <SDL2/SDL.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include "pointers.hpp"

static Uint32 wakeup_on_mpv_redraw, wakeup_on_mpv_events;

int main( int argc, char *argv[] )
{
    constexpr char USAGE[]{
        R"(toyunda-player.
     Usage:
         toyunda-player [options] <file>

     Options:
         --invert  invert the screen
    )"};

    std::map< std::string, docopt::value > args{docopt::docopt(
        USAGE, {argv + 1, argv + argc}, true, "toyunda-player" )};
    auto const mediaFile = args["<file>"].asString();
    const int factor     = args["--invert"].asBool() ? 1 : -1;

    std::unique_ptr<mpv_handle> mpv(mpv_create());
    if (!mpv){
        throw std::runtime_error("mpv context init failed");
    }
    if (mpv_initialize(mpv.get()) < 0){
        throw std::runtime_error("mpv initialization fail");
    }

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        throw std::runtime_error("failed to init SDL");
    }
    std::unique_ptr<SDL_Window> window(SDL_CreateWindow(
        "Toyunda Player",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,
        720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE ));
    if( !window ) {
        throw std::runtime_error("failed to create SDL window");
    }
    auto ptr = (mpv_opengl_cb_context*) mpv_get_sub_api(mpv.get(), MPV_SUB_API_OPENGL_CB) ;
    std::unique_ptr<mpv_opengl_cb_context> mpv_gl(ptr);
    if (!mpv_gl){
        throw std::runtime_error("failed to create mpv GL API handle");
    }
    // retrieve opengl context DO NOT DELETE
    SDL_GLContext glcontext = SDL_GL_CreateContext( window.get() );
    if( !glcontext ) {
        throw std::runtime_error("failed to create GL context");
    }
    // ^ DO NOT DELETE ^

    mpv_opengl_cb_init_gl( mpv_gl.get(), nullptr, [](void *, const char *name){
        return SDL_GL_GetProcAddress( name );
    }, nullptr);

    mpv_set_option_string( mpv.get(), "vo", "opengl-cb" );

    wakeup_on_mpv_events = SDL_RegisterEvents( 1 );
    mpv_set_wakeup_callback( mpv.get(), [](void*) {
        SDL_Event event ;
        event.type = wakeup_on_mpv_events;
        SDL_PushEvent( &event );
    }, nullptr );

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


    while (!finished) {
        SDL_Event event;
        if (SDL_WaitEvent(&event) != 1){
            throw std::runtime_error("event loop error");
        }
        bool redraw = false;
        switch (event.type) {
        case SDL_QUIT:
            finished = true ;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                redraw = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_SPACE){
                mpv_command_string(mpv.get(), "cycle pause");
            }
            break;
        default:

            if (event.type == wakeup_on_mpv_redraw){
                redraw = true;
            }
            if (event.type == wakeup_on_mpv_events) {
                while (1) {
                    mpv_event *mp_event = mpv_wait_event(mpv.get(), 0);
                    if (mp_event->event_id == MPV_EVENT_NONE){
                        break;
                    }
                    std::cout << "event: " << mpv_event_name(mp_event->event_id) << std::endl ;
                }
            }
        }
        if (redraw) {
            int w, h;
            SDL_GetWindowSize(window.get(), &w, &h);
            mpv_opengl_cb_draw(mpv_gl.get(), 0, w, -h);
            SDL_GL_SwapWindow(window.get());
        }
    }

    /*
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
    */
    SDL_Quit();
    return 0;
}
