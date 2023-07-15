SET(SOURCES screens/ui_start.c
    screens/ui_userScreen.c
    screens/ui_passwordScreen.c
    screens/ui_adminScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    fonts/ui_font_chinese.c
    fonts/ui_font_chinese24.c)

add_library(ui ${SOURCES})
