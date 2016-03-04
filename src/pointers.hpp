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
                if (mpv != nullptr){
                    mpv_terminate_destroy(mpv);
                }
            } ;
    };

    template <>
    class default_delete <mpv_opengl_cb_context>{
        public:
            void operator()(mpv_opengl_cb_context* mpv_gl) const {
                if (mpv_gl != nullptr){
                    mpv_opengl_cb_uninit_gl(mpv_gl);
                }
            } ;
    };

    template <>
    class default_delete <SDL_Window>{
        public:
            void operator()(SDL_Window* window) const {
                if (window != nullptr){
                    SDL_DestroyWindow(window);
                }
            } ;
    };

    template <>
    class default_delete <SDL_Surface>{
        public:
            void operator()(SDL_Surface* surface) const {
                if (surface != nullptr){
                    SDL_FreeSurface(surface);
                }
            }
    };

    template <>
    class default_delete <SDL_Texture>{
        public:
            void operator()(SDL_Texture* texture) const {
                if (texture != nullptr){
                    SDL_DestroyTexture(texture);
                }
            }
    };
}   
