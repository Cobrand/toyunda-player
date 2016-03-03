#include "pointers.hpp"
/*
#include <iostream>
namespace std{
    template <>
    class default_delete <mpv_handle>{
        public:
            void operator()(mpv_handle* mpv) const {
                mpv_terminate_destroy(mpv);
                std::cout << "mpv_handle destroyed" << std::endl ;
            } ;
    };

    template <>
    class default_delete <mpv_opengl_cb_context>{
        public:
            void operator()(mpv_opengl_cb_context* mpv_gl) const {
                mpv_opengl_cb_uninit_gl(mpv_gl);
                std::cout << "mpv_opengl_cb_context destroyed" << std::endl ;
            } ;
    };

    template <>
    class default_delete <SDL_Window>{
        public:
            void operator()(SDL_Window* window) const {
                SDL_DestroyWindow(window);
                std::cout << "window destroyed" << std::endl ;
            } ;
    };
}
*/
