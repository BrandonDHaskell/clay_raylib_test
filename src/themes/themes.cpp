// themes.cpp
#include "themes.h"

const Theme LightTheme = {
    // Navbar Colors
    {30, 30, 60, 255},      // navBarColor
    {70, 90, 160, 255},     // tabActiveColor
    {50, 60, 90, 255},      // tabInactiveColor

    // Content Colors
    {50, 60, 90, 255},      // headerBackgroundColor
    {80, 80, 115, 255},     // contentBackgroundColor

    // Section Colors
    {210, 225, 255, 255},   // recordDetailsBoxColor
    {230, 245, 250, 255},   // recordCrudBoxColor
    {240, 240, 250, 255},   // recordsDisplayBoxColor

    // Text Colors
    WHITE,                  // tabLabelTextColor
    DARKBLUE,               // headerTextColor
    DARKGRAY,               // contentTextColor

    // Font Sizes
    14,                     // tabFontSize
    20,                     // headerFontSize
    24                      // contentFontSize
};

// TODO: Update colors to actual Dark theme colors
const Theme DarkTheme = {
    // Navbar Colors
    {70, 90, 160, 255},     // navBarColor
    {30, 30, 60, 255},      // tabActiveColor
    {50, 60, 90, 255},      // tabInactiveColor

    // Content Colors
    {50, 60, 90, 255},      // headerBackgroundColor
    {245, 245, 255, 255},   // contentBackgroundColor

    // Section Colors
    {210, 225, 255, 255},   // recordDetailsBoxColor
    {230, 245, 250, 255},   // recordCrudBoxColor
    {240, 240, 250, 255},   // recordsDisplayBoxColor

    // Text Colors
    WHITE,                  // tabLabelTextColor
    DARKBLUE,               // headerTextColor
    DARKGRAY,               // contentTextColor

    // Font Sizes
    14,                     // tabFontSize
    20,                     // headerFontSize
    24                      // contentFontSize
};

// Mutable global pointer for theme swaps
const Theme* CurrentTheme = &LightTheme;