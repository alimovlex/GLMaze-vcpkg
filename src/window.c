/* Copyright (C) 2016 ultitech - All Rights Reserved

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
*/

#include "window.h"
#include "config.h"

#include <SDL.h>

static SDL_Window *window;
static SDL_GLContext context;

static int screen_size[2];
static KeypressHandler keypress_handlers[16];
static unsigned int keypress_handlers_count = 0;

void window_init()
{
    // Force SDL to capture all keyboard shortcuts (including Super/Win keys)
    SDL_SetHint(SDL_HINT_GRAB_KEYBOARD, "1");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    screen_size[0] = config_get_value_integer("res_width", 3840);
    screen_size[1] = config_get_value_integer("res_height", 2160);

    char fullscreen = config_get_value_integer("fullscreen", 1);

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALWAYS_ON_TOP;

    if (fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    window = SDL_CreateWindow(
        "GLMaze",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screen_size[0],
        screen_size[1],
        flags
    );

    if (!window)
    {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    context = SDL_GL_CreateContext(window);

    if (!context)
    {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    char vsync = config_get_value_integer("vsync", 1);
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);

    SDL_GetWindowSize(window, &screen_size[0], &screen_size[1]);

    // Enforce cursor hiding and window grabbing directly
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void window_quit()
{
    // Restore normal pointer behavior before closing
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_SetWindowGrab(window, SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);

    if (context)
        SDL_GL_DeleteContext(context);

    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

void window_add_keypress_handler(KeypressHandler handler)
{
    if (keypress_handlers_count < 16)
    {
        keypress_handlers[keypress_handlers_count++] = handler;
    }
}

int window_do_events()
{
    SDL_Event ev;

    static int space_down  = 0;
    static int delete_down = 0;
    static int a_down      = 0;

    while (SDL_PollEvent(&ev))
    {
        if (ev.type == SDL_QUIT)
        {
            return 0;
        }

        // Drop mouse input completely
        if (ev.type == SDL_MOUSEMOTION ||
            ev.type == SDL_MOUSEBUTTONDOWN ||
            ev.type == SDL_MOUSEBUTTONUP ||
            ev.type == SDL_MOUSEWHEEL)
        {
            continue;
        }

        if (ev.type == SDL_KEYDOWN)
        {
            SDL_Keycode key = ev.key.keysym.sym;

            if (key == SDLK_ESCAPE ||
                key == SDLK_LGUI ||
                key == SDLK_RGUI)
            {
                continue;
            }

            if (key == SDLK_SPACE)
                space_down = 1;
            else if (key == SDLK_DELETE)
                delete_down = 1;
            else if (key == SDLK_a)
                a_down = 1;

            // Unlock combo (Space + Delete + A)
            if (space_down && delete_down && a_down)
            {
                return 0;
            }

            if (!ev.key.repeat)
            {
                for (unsigned int i = 0; i < keypress_handlers_count; i++)
                {
                    keypress_handlers[i](key);
                }
            }
        }

        if (ev.type == SDL_KEYUP)
        {
            SDL_Keycode key = ev.key.keysym.sym;

            if (key == SDLK_SPACE)
                space_down = 0;
            else if (key == SDLK_DELETE)
                delete_down = 0;
            else if (key == SDLK_a)
                a_down = 0;
        }
    }

    return 1;
}

void window_swap_buffers()
{
    SDL_GL_SwapWindow(window);
}

void window_get_size(int size[2])
{
    size[0] = screen_size[0];
    size[1] = screen_size[1];
}

//cmake . -B build --preset linux-g++-debug && cmake --build build -j4
