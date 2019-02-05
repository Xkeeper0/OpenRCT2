/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Language.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/network/network.h>

static int32_t ResizeLanguage = LANGUAGE_UNDEFINED;

// clang-format off
enum WINDOW_DEBUG_DESYNC_WIDGET_IDX
{
    WIDX_BACKGROUND,
};

#define WINDOW_WIDTH    (400)
#define WINDOW_HEIGHT   (8 + 15 + 15 + 15 + 15 + 15 + 11 + 8)

static rct_widget window_debug_desync_widgets[] = {
    { WWT_FRAME,    0,  0,  WINDOW_WIDTH - 1,   0,              WINDOW_HEIGHT - 1,  STR_NONE,                               STR_NONE },
//    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 0,     8 + 15 * 0 + 11,    STR_DEBUG_PAINT_SHOW_WIDE_PATHS,        STR_NONE },
//    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 1,     8 + 15 * 1 + 11,    STR_DEBUG_PAINT_SHOW_BLOCKED_TILES,     STR_NONE },
//    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 2,     8 + 15 * 2 + 11,    STR_DEBUG_PAINT_SHOW_SEGMENT_HEIGHTS,   STR_NONE },
//    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 3,     8 + 15 * 3 + 11,    STR_DEBUG_PAINT_SHOW_BOUND_BOXES,       STR_NONE },
//    { WWT_CHECKBOX, 1,  8,  WINDOW_WIDTH - 8,   8 + 15 * 4,     8 + 15 * 4 + 11,    STR_DEBUG_PAINT_SHOW_DIRTY_VISUALS,     STR_NONE },
    { WIDGETS_END },
};

//static void window_debug_desync_mouseup(rct_window * w, rct_widgetindex widgetIndex);
static void window_debug_desync_invalidate(rct_window * w);
static void window_debug_desync_paint(rct_window * w, rct_drawpixelinfo * dpi);

static rct_window_event_list window_debug_desync_events = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_debug_desync_invalidate,
    window_debug_desync_paint,
    nullptr
};
// clang-format on

rct_window* window_debug_desync_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_DEBUG_DESYNC);
    if (window != nullptr)
        return window;

    window = window_create(
        16, 33, WINDOW_WIDTH, WINDOW_HEIGHT, &window_debug_desync_events, WC_DEBUG_DESYNC, WF_STICK_TO_FRONT | WF_TRANSPARENT);

    window->widgets = window_debug_desync_widgets;
    window->enabled_widgets = 0;
    window_init_scroll_widgets(window);
    window_push_others_below(window);

    window->colours[0] = TRANSLUCENT(COLOUR_BLACK);
    window->colours[1] = COLOUR_GREY;

    ResizeLanguage = LANGUAGE_UNDEFINED;
    return window;
}

/*
static void window_debug_desync_mouseup([[maybe_unused]] rct_window* w, rct_widgetindex widgetIndex)
{
}
*/
static void window_debug_desync_invalidate(rct_window* w)
{
    window_invalidate(w);
}

static void window_debug_desync_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    if (network_get_mode() != NETWORK_MODE_CLIENT)
        return;

    window_draw_widgets(w, dpi);
    int32_t x = w->x + 2;
    int32_t y = w->y + 2;

    NetworkSyncInfo _network_sync_info = network_get_sync_info();
    char buf[512] = {};

    auto _old_font = gCurrentFontSpriteBase;
    gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;

    sprintf(buf, "%d", _network_sync_info.tick);
    int32_t width = gfx_get_string_width(buf);
    gfx_draw_string(dpi, "Network Sync Info     Tick", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, COLOUR_WHITE, x + 200 - width, y);

    if (!network_is_desynchronised())
        gfx_draw_string(dpi, "Synchronized", COLOUR_BRIGHT_GREEN | COLOUR_FLAG_OUTLINE, x + 300, y);
    else
        gfx_draw_string(dpi, "Desynchronized", COLOUR_SATURATED_RED | COLOUR_FLAG_OUTLINE, x + 300, y);

    y += 12;

    uint8_t colour = COLOUR_WHITE;
    if (_network_sync_info.client_srand0 != _network_sync_info.server_srand0)
        colour = (COLOUR_BRIGHT_RED | COLOUR_FLAG_OUTLINE);

    sprintf(buf, "%08X", _network_sync_info.client_srand0);
    gfx_draw_string(dpi, "Client srand:", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, colour, x + 100, y);
    y += 12;

    sprintf(buf, "%08X", _network_sync_info.server_srand0);
    gfx_draw_string(dpi, "Server srand:", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, colour, x + 100, y);
    y += 12;

    colour = COLOUR_WHITE;
    if (_network_sync_info.client_sprite_hash != _network_sync_info.server_sprite_hash)
        colour = (COLOUR_BRIGHT_RED | COLOUR_FLAG_OUTLINE);

    sprintf(buf, "%d", _network_sync_info.last_checksum_tick);
    width = gfx_get_string_width(buf);
    gfx_draw_string(dpi, "Last checksum tick:", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, colour, x + 200 - width, y);
    y += 12;

    sprintf(buf, "%s", _network_sync_info.client_sprite_hash.c_str());
    gfx_draw_string(dpi, "Client hash:", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, colour, x + 100, y);
    y += 12;

    sprintf(buf, "%s", _network_sync_info.server_sprite_hash.c_str());
    gfx_draw_string(dpi, "Server hash:", COLOUR_WHITE, x, y);
    gfx_draw_string(dpi, buf, colour, x + 100, y);

    gCurrentFontSpriteBase = _old_font;
}
