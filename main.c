#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/randr.h>
#include <jmorecfg.h>
#include <stdbool.h>
#include <Xos.h>

int nbrMonitors(xcb_connection_t *con, xcb_window_t root) {
    xcb_randr_monitor_info_t *infoMonitor;
    xcb_randr_get_monitors_cookie_t cookie;
    xcb_randr_get_monitors_reply_t *monitorsReply;
    xcb_randr_monitor_info_iterator_t monitorIterator;

    // Xrandr get monitors
    uint8_t active = 0; // use ?
    cookie = xcb_randr_get_monitors(con, root, active);
    monitorsReply = xcb_randr_get_monitors_reply(con, cookie, NULL);
    // They all seem to be the same
    //printf("Number of monitors : %d\n", test->nMonitors);
    //printf("Number of monitors : %d\n", test->nOutputs);

    monitorIterator = xcb_randr_get_monitors_monitors_iterator(monitorsReply);

    printf("----Randr monitors info----\n");
    while (monitorIterator.rem > 0) {
        infoMonitor = monitorIterator.data;
        printf("Monitor name: %d\n", infoMonitor->name);
        printf("Monitor nbr: %d\n", monitorIterator.rem);
        printf("Primary: %d\n", infoMonitor->primary);
        printf("%dx%d\n", infoMonitor->width, infoMonitor->height);
        printf("x offset: %d, y offset: %d\n\n", infoMonitor->x, infoMonitor->y);
        xcb_randr_monitor_info_next(&monitorIterator);
    }
    printf("---------------------------\n");

    return xcb_randr_get_monitors_monitors_length(monitorsReply);
}

int main () {
    xcb_connection_t        *con;               // Connection
    xcb_screen_t            *screen;            // Screen
    int                     screen_nbr;         // Screen number
    //xcb_screen_iterator_t   iter;               // Screen iterator
    xcb_window_t            root;                // Window
    uint32_t                value_mask;
    uint32_t                value_list[2];
    boolean                 finished = false;
    xcb_generic_event_t     *event;
    xcb_key_press_event_t   *btnPressed;

    printf("XCB tests:\n");

    // Opening a connection to the X server
    printf("Opening connection\n\n");
    con = xcb_connect(NULL, &screen_nbr);
    if (xcb_connection_has_error(con)) {
        printf("Error opening display");
        exit(1);
    }

    // Always returns 1 for me ??
    //printf("%d\n", xcb_setup_roots_iterator(xcb_get_setup(con)).rem);
    // Get the number of screens
    /*iter = xcb_setup_roots_iterator(xcb_get_setup(con));
    for (; iter.rem; --screen_nbr, xcb_screen_next(&iter)) {
        if (screen_nbr == 0) {
            screen = iter.data;
            break;
        }
    }*/

    screen = xcb_setup_roots_iterator(xcb_get_setup(con)).data;
    printf("Screen %d:\n", screen->root);
    printf("%dx%d\n", screen->width_in_pixels, screen->height_in_pixels);

    printf("Number of monitors : %d\n", nbrMonitors(con, screen->root)); // Returns the real number of screens

    // Ask for a window's id
    root = xcb_generate_id(con);
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->white_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                    XCB_EVENT_MASK_KEY_PRESS;

    // Creating the window
    xcb_create_window(con, // Connection
                      XCB_COPY_FROM_PARENT, // Depth
                      root, // Id
                      screen->root, // Parent window
                      0,0, // x,y
                      screen->width_in_pixels,screen->height_in_pixels, // Size: w,h
                      0, // Border width
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, // Class
                      screen->root_visual, // Visual
                      value_mask, value_list);

    /* set the title of the window */

    char *title = "SYCBWM";
    xcb_change_property (con,
                         XCB_PROP_MODE_REPLACE,
                         root,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         strlen(title),
                         title);

    // Messing with window tree
    xcb_query_tree_cookie_t cookie_tree = xcb_query_tree(con, root);
    xcb_query_tree_reply_t *reply_tree = xcb_query_tree_reply(con, cookie_tree, NULL);
    printf("Size of tree : %u\n", xcb_query_tree_children_length(reply_tree));
    xcb_window_t *test_child = xcb_query_tree_children(reply_tree);


    // Map window on the screen
    xcb_map_window(con, root);
    xcb_flush(con);

    // Event loop
    while(!finished && (event = xcb_wait_for_event(con))) {
        switch(event->response_type) {
            case XCB_KEY_PRESS:
                btnPressed = ((xcb_key_press_event_t*)event);
                printf("\nKeycode: %d\n", btnPressed->detail);

                if (btnPressed->detail == 9) { // Finish if escape is pressed
                    /* ESC */
                    finished = true;
                } else if (btnPressed->detail == 30) {
                    /* U */
                    // Doesn't work with root window which is under another window manager ?
                    // Doesn't work because it simply isn't a subwindow ?
                    printf("Unmapping window\n");
                    xcb_unmap_window(con, root);
                    sleep(5);
                    printf("Re-Mapping window");
                    xcb_map_window(con, root);
                }
                break;
            case XCB_BUTTON_PRESS:
                // Printing information about button pressed
                printf("\nButton pressed: %u\n", ((xcb_button_press_event_t*)event)->detail);
                printf("X-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_x);
                printf("Y-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_y);
                break;
            case XCB_EXPOSE:
                break;
            default:
                printf("something else\n");
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