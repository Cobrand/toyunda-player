#pragma once
#include <memory>
#include <SDL2/SDL.h>
#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <iostream>
namespace std{
    template <>
    class default_delete <mpv_handle>{
        public:
            void operator()(mpv_handle* mpv) const {
                mpv_terminate_destroy(mpv);
            } ;
    };

    template <>
    class default_delete <mpv_opengl_cb_context>{
        public:
            void operator()(mpv_opengl_cb_context* mpv_gl) const {
                mpv_opengl_cb_uninit_gl(mpv_gl);
            } ;
    };

    template <>
    class default_delete <SDL_Window>{
        public:
            void operator()(SDL_Window* window) const {
                SDL_DestroyWindow(window);
            } ;
    };
}
