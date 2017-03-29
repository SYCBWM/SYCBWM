#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <jmorecfg.h>
#include <stdbool.h>

int main () {
    xcb_connection_t        *con;               // Connection
    xcb_screen_t            *screen;            // Screen
    int                     screen_nbr;         // Screen number
    xcb_screen_iterator_t   iter;               // Screen iterator
    xcb_window_t            win;                // Window
    uint32_t                value_mask;
    uint32_t                value_list[2];
    boolean                 finished = false;
    xcb_generic_event_t     *event;
    xcb_key_press_event_t   *btnPressed;

    printf("XCB tests:\n");

    // Opening a connection to the X server
    printf("Opening connection\n");
    con = xcb_connect(NULL, &screen_nbr);
    if (xcb_connection_has_error(con)) {
        printf("Error opening display");
        exit(1);
    }

    printf("%d\n", xcb_setup_roots_iterator(xcb_get_setup(con)).rem);

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
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->white_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                    XCB_EVENT_MASK_KEY_PRESS;

    // Creating the window
    xcb_create_window(con, // Connection
                      XCB_COPY_FROM_PARENT, // Depth
                      win, // Id
                      screen->root, // Parent window
                      0,0, // x,y
                      screen->width_in_pixels,screen->height_in_pixels, // Size: w,h
                      0, // Border width
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, // Class
                      screen->root_visual, // Visual
                      value_mask, value_list);

    // Map window on the screen
    xcb_map_window(con, win);
    xcb_flush(con);

    while(!finished && (event = xcb_wait_for_event(con))) {
        switch(event->response_type) {
            case XCB_KEY_PRESS:
                btnPressed = ((xcb_key_press_event_t*)event);
                printf("Keycode: %d\n", btnPressed->detail);
                /* ESC */
                if (btnPressed->detail == 9) { // Finish if escape is pressed
                    finished = true;
                }
                break;
            case XCB_BUTTON_PRESS:
                printf("Button pressed: %u\n", ((xcb_button_press_event_t*)event)->detail);
                printf("X-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_x);
                printf("Y-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_y);
                break;
            case XCB_EXPOSE:
                break;
        }
        free(event);
    }

    //sleep(5); // Waits 5 seconds

    // Closing the connection to the X server
    printf("\nDisconnecting");
    xcb_disconnect(con);

    return 0;
}