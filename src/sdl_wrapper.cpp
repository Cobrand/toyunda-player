#include "sdl_wrapper.hpp"

#include <iostream>

void free_sdl_window::operator()( free_sdl_window::ptr &win )
{
    SDL_DestroyWindow( win );
}

namespace SDL
{
void GetWindowSize( Window_ptr &window, int &w, int &h )
{
    SDL_GetWindowSize( window.get(), &w, &h );
}

void redraw( Window_ptr &window, MPV::openGL_CB_context &mpv_gl, int factor )
{
    int w, h;
    GetWindowSize( window, w, h );
    mpv_opengl_cb_draw( mpv_gl.get(), 0, w, factor * h );
    //glViewport(100,100,100,100);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // OPEN GL CODE HERE
    //
    SDL_GL_SwapWindow( window.get() );
}

void Event_Dispatcher::register_event( Event_Dispatcher::Event_type e,
                                       Event_Dispatcher::Callback cb )
{
    events[e] = std::move( cb );
}

Event_Dispatcher::Result Event_Dispatcher::handle( SDL_Event &evts )
{
    if( events.find( evts.type ) == events.end() ) {
        return Result::none;
    }
    Event_type e = evts.type;
    return events[e]( evts );
}
Event_Dispatcher build( MPV::Handle_ptr &mpv,
                        Uint32 wakeup_on_mpv_redraw,
                        void ( *on_mpv_redraw )( void * ),
                        Uint32 wakeup_on_mpv_events,
                        void ( *on_mpv_events )( void * ),
                        double &speed )
{
    SDL::Event_Dispatcher handler;
    handler.register_event(
        SDL_QUIT,
        []( SDL_Event & ) { return SDL::Event_Dispatcher::Result::finished; } );
    handler.register_event(
        SDL_WINDOWEVENT,
        []( SDL_Event &evt ) {
            if( evt.window.event == SDL_WINDOWEVENT_EXPOSED ) {
                return SDL::Event_Dispatcher::Result::redraw;
            }
            return SDL::Event_Dispatcher::Result::none;
        } );
    handler.register_event(
        SDL_KEYDOWN,
        [&mpv, &speed]( SDL_Event &evt ) {
            if( evt.key.keysym.sym == SDLK_SPACE ) {
                mpv_command_string( mpv.get(), "cycle pause" );
            } else if( evt.key.keysym.sym == SDLK_KP_PLUS ) {
                speed += .5;
                mpv_set_property(
                    mpv.get(), "speed", MPV_FORMAT_DOUBLE, &speed );
            } else if( evt.key.keysym.sym == SDLK_KP_MINUS ) {
                speed -= .5;
                mpv_set_property(
                    mpv.get(), "speed", MPV_FORMAT_DOUBLE, &speed );
            }
            return SDL::Event_Dispatcher::Result::none;
        } );
    handler.register_event( wakeup_on_mpv_redraw,
                            []( SDL_Event &evt ) {
                                return SDL::Event_Dispatcher::Result::redraw;
                            } );
    handler.register_event(
        wakeup_on_mpv_events,
        [&mpv]( SDL_Event & ) {
            while( 1 ) {
                mpv_event *mp_event = mpv_wait_event( mpv.get(), 0 );
                if( mp_event->event_id == MPV_EVENT_NONE ) {
                    break;
                }
                std::cout << mpv_event_name( mp_event->event_id ) << "\n";
            }
            return SDL::Event_Dispatcher::Result::none;
        } );
    return std::move( handler );
}
}
