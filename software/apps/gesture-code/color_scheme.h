#pragma once

// Color scheme enumeration
typedef enum
{
    SCHEME_DEFAULT,  // Blue → Green → Red
    SCHEME_ALTERNATE // Magenta → White → Yellow
} ColorScheme;

// Global color scheme state
extern ColorScheme current_scheme;