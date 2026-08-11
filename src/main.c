
/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */
#define _GNU_SOURCE

#include "drawer.h"
#include "window.h"
#include "scene.h"
#include "config.h"
#include "file.h"
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <linux/limits.h>

#ifdef _WIN32
#include <string.h>
#include <windows.h>
#include <direct.h>
#endif

#include <SDL.h> //for SDL_GetTicks
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#if defined SCREENSAVER
enum screensaverParameter { NONE, CONFIGURATION, PREVIEW, FULLSCREEN };

static enum screensaverParameter get_screensaver_parameter(int argc, const char *argv[])
{
	if(argc == 1) return CONFIGURATION;
	else if(argc == 2)
	{
		char* arg = argv[1];
		if(arg[0] == '-' || arg[0] == '/')
		{
			switch(tolower(arg[1]))
			{
				case 'c':
					return CONFIGURATION;
				case 'p':
					return PREVIEW; // Preview in Settings screen
				case 's':
					return FULLSCREEN; // When user clicks Preview button in Settings screen
			}
		}
	}

	return NONE;
}
#endif

static void run()
{
	config_load();
	config_print();

	window_init();
	drawer_init();
	scene_init();

	float time_passed = 0.0;

	while(window_do_events())
	{
		int start = SDL_GetTicks();

		scene_update(time_passed);

		drawer_begin_scene(time_passed);
		scene_draw();
		drawer_end_scene();

		int end = SDL_GetTicks();
		time_passed = (end-start)/1000.0;
	}

	scene_quit();
	drawer_quit();
	window_quit();
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	// Get the directory where the executable is located
	char exe_path[PATH_MAX];
	char exe_dir[PATH_MAX];

#ifdef __linux__
	ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path)-1);
	if(len != -1)
	{
		exe_path[len] = '\0';
		char *dir = dirname(exe_path);
		strcpy(exe_dir, dir);
		strcat(exe_dir, "/");
	}
	else
	{
		strcpy(exe_dir, "./");
	}
#elif _WIN32
	GetModuleFileName(NULL, exe_path, sizeof(exe_path));
	char *dir = dirname(exe_path);
	strcpy(exe_dir, dir);
	strcat(exe_dir, "\\");
#elif __APPLE__
	CFURLRef url = CFBundleCopyExecutableURL(CFBundleGetMainBundle());
	CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	CFStringGetCString(path, exe_path, sizeof(exe_path), kCFStringEncodingUTF8);
	CFRelease(path);
	CFRelease(url);
	char *dir = dirname(exe_path);
	strcpy(exe_dir, dir);
	strcat(exe_dir, "/");
#else
	strcpy(exe_dir, "./");
#endif

	// Set resource and output directories to the executable's directory
	file_set_resource_dir(exe_dir);
	file_set_output_dir(exe_dir);

	// Also try to load config from executable directory
	// The config_load() function expects config.txt in the current working directory
	// We need to change the current working directory to the executable's directory
	if(chdir(exe_dir) != 0)
	{
		// If we can't change directory, try to use the current directory
		// but the resources might not be found
		fprintf(stderr, "Warning: Could not change to executable directory: %s\n", exe_dir);
	}

	run();

	return 0;
}
