#include "mpv_wrapper.hpp"

#include <stdexcept>
#include <map>

namespace MPV
{
Handle_ptr::Handle_ptr() : mpv{mpv_create()}
{
    if( !mpv ) {
        throw std::runtime_error( "context init failed" );
    }
    if( mpv_initialize( mpv ) < 0 ) {
        throw std::runtime_error( "mpv init failed" );
    }
}
Handle_ptr::type const *Handle_ptr::get() const { return mpv; }
Handle_ptr::type *Handle_ptr::get() { return mpv; }
Handle_ptr::~Handle_ptr() { mpv_terminate_destroy( mpv ); }

mpv_opengl_cb_context *get_sub_api( mpv_handle_ptr mpv, sub_api api )
{
    using type = mpv_opengl_cb_context;
    static std::map< sub_api, mpv_sub_api > conv_table{
        {sub_api::OPENGL_CB, MPV_SUB_API_OPENGL_CB}};
    return static_cast< type * >(
        mpv_get_sub_api( mpv.get(), conv_table[api] ) );
}

openGL_CB_context::openGL_CB_context( mpv_handle_ptr mpv, sub_api api )
    : mpv_gl{get_sub_api( mpv, api )}
{
    if( !mpv_gl ) {
        throw std::runtime_error( "failed to create mpv GL API handle" );
    }
}
openGL_CB_context::type const *openGL_CB_context::get() const { return mpv_gl; }
openGL_CB_context::type *openGL_CB_context::get() { return mpv_gl; }
openGL_CB_context::~openGL_CB_context() { mpv_opengl_cb_uninit_gl( mpv_gl ); }
}
