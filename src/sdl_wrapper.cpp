#include "sdl_wrapper.hpp"

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
}
