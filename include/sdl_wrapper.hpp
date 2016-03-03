#pragma once

#include <memory>
#include <functional>
#include <map>

#include "mpv_wrapper.hpp"

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
}

struct free_sdl_window {
    using ptr = SDL_Window *;
    void operator()( ptr &win );
};

template < typename T >
constexpr std::size_t as_index( T t )
{
    return static_cast< std::size_t >( t );
}

namespace SDL
{
using Window_ptr = std::unique_ptr< SDL_Window, free_sdl_window >;
void GetWindowSize( Window_ptr &window, int &w, int &h );
void redraw( Window_ptr &window, MPV::openGL_CB_context &mpv_gl, int factor );

class Event_Dispatcher
{
  public:
    enum class Result : uint { redraw, finished, none, _MAX };

    using Event_type = decltype( std::declval< SDL_Event >().type );
    using Callback   = std::function< Result( SDL_Event & ) >;

    // voir si on met des signaux (boost::signal2)
    Event_Dispatcher() = default;

    void register_event( Event_type e, Callback cb );
    Result handle( SDL_Event &evts );

    ~Event_Dispatcher() = default;

  private:
    std::map< Event_type, Callback > events;
};
Event_Dispatcher build( MPV::Handle_ptr &mpv,
                        Uint32 redraw_,
                        void ( *redraw_f )( void * ),
                        Uint32 evts,
                        void ( *evts_f )( void * ),
                        double &speed );
}
