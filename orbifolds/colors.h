#pragma once

#define RGBA(r, g, b, a) r | (g << 8) | (b << 16) | (a << 24)

namespace window {
	inline uint32_t color_transparent		= RGBA(0, 0, 0, 0);

	inline uint32_t color_consoleCommand	= RGBA(255, 255, 255, 255);
	inline uint32_t color_consoleMessage	= RGBA(127, 127, 127, 255);
	inline uint32_t color_consoleWarning	= RGBA(127, 127,   0, 255);
	inline uint32_t color_consoleError		= RGBA(127,   0,   0, 255);
	inline uint32_t color_consoleCursor		= RGBA(255, 255, 255, 255);
	inline uint32_t color_consoleHighlight	= RGBA( 50,  50,  50,  255);

	inline uint32_t color_guiWarning		= RGBA(255, 255,   0, 255);
	inline uint32_t color_guiError			= RGBA(255,   0,   0, 255);

	inline uint32_t color_stagingHighlight	= RGBA(85, 85, 85, 255);
}