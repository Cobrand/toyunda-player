// Build with: gcc -o main main.c `pkg-config --libs --cflags mpv sdl2`

#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>

#include <mpv/client.h>
#include <mpv/opengl_cb.h>

int puissance2sup(int i)
{
    double logbase2 = log(i) / log(2);
    return (int) floor(pow(2.0, ceil(logbase2)));
}


static Uint32 wakeup_on_mpv_redraw, wakeup_on_mpv_events;

static void *get_proc_address_mpv(void *fn_ctx, const char *name)
{
    return SDL_GL_GetProcAddress(name);
}

static void on_mpv_events(void *ctx)
{
    SDL_Event event = {.type = wakeup_on_mpv_events};
    SDL_PushEvent(&event);
}

static void on_mpv_redraw(void *ctx)
{
    SDL_Event event = {.type = wakeup_on_mpv_redraw};
    SDL_PushEvent(&event);
}

#define SHAPE_SIZE 16
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
LPSTR szCmdLine, int iCmdShow){
	/*
	return 0 ;
}

int main(int argc, char *argv[])
{
	*/
	char * argv[] = {"toyunda_player","jojo.avi"};
	int argc = 2 ;
	
    if (argc != 2)
        throw "pass a single media file as argument";

    mpv_handle *mpv = mpv_create();
    if (!mpv){
        throw "context init failed";
    }

    // Some minor options can only be set before mpv_initialize().
    if (mpv_initialize(mpv) < 0){
        throw "mpv init failed";
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        throw "SDL init failed";
    }
    if (TTF_Init() < 0 ){
        throw "TTF init failed";
    }

    SDL_Window *window =
        SDL_CreateWindow("Toyunda Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         1000, 500, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
                                    SDL_WINDOW_RESIZABLE);
    if (!window){
        throw "failed to create SDL window";
    }
	
    auto *font = TTF_OpenFont("DEJAVUSANSMONO-BOLD.TTF",72);

    // The OpenGL API is somewhat separate from the normal mpv API. This only
    // returns NULL if no OpenGL support is compiled.
    mpv_opengl_cb_context *mpv_gl = (mpv_opengl_cb_context*) mpv_get_sub_api(mpv, MPV_SUB_API_OPENGL_CB);
    if (!mpv_gl){
        throw "failed to create mpv GL API handle";
    }


    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    if (!glcontext){
        throw "failed to create SDL GL context";
    }

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // This makes mpv use the currently set GL context. It will use the callback
    // to resolve GL builtin functions, as well as extensions.
    if (mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address_mpv, NULL) < 0){
        throw "failed to initialize mpv GL context";
    }

    // Actually using the opengl_cb state has to be explicitly requested.
    // Otherwise, mpv will create a separate platform window.
    if (mpv_set_option_string(mpv, "vo", "opengl-cb") < 0){
        throw "failed to set VO";
    }

    // We use events for thread-safe notification of the SDL main loop.
    // Generally, the wakeup callbacks (set further below) should do as least
    // work as possible, and merely wake up another thread to do actual work.
    // On SDL, waking up the mainloop is the ideal course of action. SDL's
    // SDL_PushEvent() is thread-safe, so we use that.
    wakeup_on_mpv_redraw = SDL_RegisterEvents(1);
    wakeup_on_mpv_events = SDL_RegisterEvents(1);
    if (wakeup_on_mpv_redraw == (Uint32)-1 || wakeup_on_mpv_events == (Uint32)-1){
        throw "could not register events";
    }

    // When normal mpv events are available.
    mpv_set_wakeup_callback(mpv, on_mpv_events, NULL);

    // When a new frame should be drawn with mpv_opengl_cb_draw().
    // (Separate from the normal event handling mechanism for the sake of
    //  users which run OpenGL on a different thread.)
    mpv_opengl_cb_set_update_callback(mpv_gl, on_mpv_redraw, NULL);

    SDL_Color text_color = SDL_Color {
        r : 255,
        g : 255,
        b : 0,
        a : 128
    };
    auto surface = TTF_RenderUTF8_Blended(font,"Hello World!",text_color);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);


    int bpp;
	Uint32 Rmask, Gmask, Bmask, Amask;
	SDL_PixelFormatEnumToMasks(
		SDL_PIXELFORMAT_ABGR8888, &bpp,
		&Rmask, &Gmask, &Bmask, &Amask
	);
    int glw = puissance2sup(surface->w);
    int glh = puissance2sup(surface->h);
    SDL_Surface *img_rgba8888 = SDL_CreateRGBSurface(0,
					glw, glh, bpp,
					Rmask, Gmask, Bmask, Amask
				);
    SDL_BlitSurface( surface, NULL, img_rgba8888, NULL );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLuint TextureID = 0;
    glGenTextures(1, &TextureID);
    std::cout << TextureID << std::endl ;
    glBindTexture(GL_TEXTURE_2D, TextureID);
    int Mode = GL_RGBA;
    std::cout << surface->w << ";" << surface->h << std::endl ;
    std::cout << surface->format->format << std::endl ;
    //if(surface->format->BytesPerPixel == 4) {
    //    Mode = GL_RGBA;
    //    std::cout << "RGBA" << std::endl ;
    //}
	
    glTexImage2D(GL_TEXTURE_2D, 0, Mode, img_rgba8888->w, img_rgba8888->h, 0, Mode, GL_UNSIGNED_BYTE, img_rgba8888->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Play this file. Note that this starts playback asynchronously.
    const char *cmd[] = {"loadfile", argv[1], NULL};
    mpv_command(mpv, cmd);
    double speed = 1.0 ;
    mpv_set_property(mpv,"speed",MPV_FORMAT_DOUBLE,&speed);
    bool finished = false ;
    while (!finished) {
        SDL_Event event;
        if (SDL_WaitEvent(&event) != 1){
            throw "event loop error";
        }

        int redraw = false;
        switch (event.type) {
            case SDL_QUIT:
                finished = true;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_EXPOSED){
                    redraw = true;
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_SPACE){
                    mpv_command_string(mpv, "cycle pause");
                }
                break;
            default:
                // Happens when a new video frame should be rendered, or if the
                // current frame has to be redrawn e.g. due to OSD changes.
                if (event.type == wakeup_on_mpv_redraw){
                    redraw = true;
                }
                // Happens when at least 1 new event is in the mpv event queue.
                if (event.type == wakeup_on_mpv_events) {
                    // Handle all remaining mpv events.
                    while (1) {
                        mpv_event *mp_event = mpv_wait_event(mpv, 0);
                        if (mp_event->event_id == MPV_EVENT_NONE){
                            break;
                        }
                        std::cout << mpv_event_name(mp_event->event_id) << "\n";
                    }
                }
        }
        if (true == finished){
            break;
        }
        if (redraw) {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            // Note:
            // - The 0 is the FBO to use; 0 is the default framebuffer (i.e.
            //   render to the window directly.
            // - The negative height tells mpv to flip the coordinate system.
            // - If you do not want the video to cover the whole screen, or want
            //   to apply any form of fancy transformation, you will have to
            //   render to a FBO.
            // - See opengl_cb.h on what OpenGL environment mpv expects, and
            //   other API details.
            mpv_opengl_cb_draw(mpv_gl, 0, w, -h);
			glViewport( 0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0,w,h,0,-1,1);
			//modelview matrix
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, TextureID);
            // For Ortho mode, of course
            int X = 0;
            int Y = 0;
            int Width = img_rgba8888->w;
            int Height = img_rgba8888->h;

            glBegin(GL_QUADS);
                glTexCoord2f(0, 0); glVertex3f(X, Y, 0);
                glTexCoord2f(1, 0); glVertex3f(X + Width, Y, 0);
                glTexCoord2f(1, 1); glVertex3f(X + Width, Y + Height, 0);
                glTexCoord2f(0, 1); glVertex3f(X, Y + Height, 0);
            glEnd();
            glDisable(GL_TEXTURE_2D);
			//glClearColor(1,1,0,1);
			//glClear(GL_COLOR_BUFFER_BIT);
            GLuint vbo;
            glBegin(GL_TRIANGLES);
             glColor3f(1,0,0);
             glVertex3f(300,200,0);
             glColor4f(0,1,0,0);
             glVertex3f(100,400,0);
             glColor3f(0,0,1);
             glVertex3f(500,400,0); //end
             glColor4f(1,1,1,1);
             glEnd();
			//SDL_RenderPresent(renderer);
			
            SDL_GL_SwapWindow(window);
        }
    }

    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    TTF_Quit();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    // Destroy the GL renderer and all of the GL objects it allocated. If video
    // is still running, the video track will be deselected.
    mpv_opengl_cb_uninit_gl(mpv_gl);

    mpv_terminate_destroy(mpv);
	
    return 0;
}
