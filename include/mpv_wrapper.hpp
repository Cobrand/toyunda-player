#pragma once

#include <stdexcept>

extern "C" {
#include <SDL.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
}

namespace MPV
{
class Handle_ptr
{
  public:
    using type = mpv_handle;

    Handle_ptr();
    type const *get() const;
    type *get();
    ~Handle_ptr();

  private:
    type *mpv;
};
void set_option_string( Handle_ptr &ptr, std::string name, std::string data );

using mpv_handle_ptr = Handle_ptr;

enum class sub_api : int { OPENGL_CB = 1 };
mpv_opengl_cb_context *get_sub_api( mpv_handle_ptr &mpv, sub_api api );

class openGL_CB_context
{
    // TODO : remplacer par un unique_ptr ???
  public:
    using type = mpv_opengl_cb_context;
    openGL_CB_context( mpv_handle_ptr &mpv, sub_api api );
    type const *get() const;
    type *get();
    ~openGL_CB_context();

  private:
    type *mpv_gl;
};
void opengl_cb_init_gl( openGL_CB_context &ctx,
                        mpv_opengl_cb_get_proc_address_fn get_proc_address,
                        void *get_proc_address_ctx = nullptr );
void opengl_cb_init_gl( openGL_CB_context &ctx,
                        std::string extensions,
                        mpv_opengl_cb_get_proc_address_fn get_proc_address,
                        void *get_proc_address_ctx = nullptr );
}
