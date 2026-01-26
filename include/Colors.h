#ifndef COLORS_H
#define COLORS_H

// ANSI color codes - Claude-inspired pastel theme
namespace Color {
    inline constexpr const char* RESET = "\033[0m";
    
    // Pastel colors (Claude-style purple theme)
    inline constexpr const char* PURPLE = "\033[38;5;141m";      // Pastel purple (primary)
    inline constexpr const char* LAVENDER = "\033[38;5;183m";    // Light lavender
    inline constexpr const char* PEACH = "\033[38;5;217m";       // Soft peach
    inline constexpr const char* MINT = "\033[38;5;158m";        // Pastel mint green
    inline constexpr const char* ROSE = "\033[38;5;211m";        // Pastel rose
    inline constexpr const char* GRAY = "\033[38;5;248m";        // Soft gray
    inline constexpr const char* GREEN = "\033[38;5;114m";       // Success green
    
    // Theme color rgb(175, 135, 255) - User's custom purple
    inline constexpr const char* THEME = "\033[38;2;175;135;255m";  // RGB theme color
    
    // Background highlights for important text
    inline constexpr const char* BG_PURPLE = "\033[48;5;141m\033[38;5;16m";  // Purple bg, dark text
    inline constexpr const char* BG_MINT = "\033[48;5;158m\033[38;5;16m";    // Mint bg, dark text
    inline constexpr const char* BG_PEACH = "\033[48;5;217m\033[38;5;16m";   // Peach bg, dark text
    inline constexpr const char* BG_ROSE = "\033[48;5;211m\033[38;5;16m";    // Rose bg, dark text
    
    inline constexpr const char* BOLD = "\033[1m";
    inline constexpr const char* DIM = "\033[2m";
}

#endif // COLORS_H
