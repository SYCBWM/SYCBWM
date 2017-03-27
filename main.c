#include <unistd.h>
#include <stdio.h>
#include <xcb/xcb.h>

int main () {
    xcb_connection_t        *con;               // Connection
    xcb_screen_t            *screen;            // Screen
    int                     screen_nbr;         // Screen number
    xcb_screen_iterator_t   iter;               // Screen iterator
    xcb_window_t            win;                // Window

    printf("XCB tests:\n");

    // Opening a connection to the X server
    printf("Opening connection\n");
    con = xcb_connect(NULL, &screen_nbr);

    // Get the number of screens
    iter = xcb_setup_roots_iterator(xcb_get_setup(con));
    for (; iter.rem; --screen_nbr, xcb_screen_next(&iter)) {
        if (screen_nbr == 0) {
            screen = iter.data;
            break;
        }
    }

    printf("\nScreen %ld:\n", screen->root);
    printf("%dx%d\n", screen->width_in_pixels, screen->height_in_pixels);

    // Ask for a window's id
    win = xcb_generate_id(con);

    // Creating the window
    xcb_create_window(con, // Connection
                      XCB_COPY_FROM_PARENT, // Depth
                      screen->root, // Parent window
                      win, // Id
                      100,100, // x,y
                      150,150, // Size: w,h
                      10, // Border width
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, // Class
                      screen->root_visual, // Visual
                      0, NULL);

    // Map window on the screen
    xcb_map_window(con, win);

    xcb_flush(con);

    pause(); // Ctrl-c to exit

    // Closing the connection to the X server
    printf("\nDisconnecting");
    xcb_disconnect(con);

    return 0;
}