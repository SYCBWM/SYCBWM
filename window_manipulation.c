#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <jmorecfg.h>

int
main ()
{
    /* open the connection to the X server */
    xcb_connection_t *connection = xcb_connect (NULL, NULL);
    boolean                 finished = false;
    xcb_generic_event_t     *event;
    xcb_key_press_event_t   *btnPressed;
    uint32_t                value_mask;
    uint32_t                value_list[2];


    /* get the first screen */
    xcb_screen_t *screen = xcb_setup_roots_iterator (xcb_get_setup (connection)).data;


    /* create the window */
    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->white_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
                    XCB_EVENT_MASK_KEY_PRESS;

    xcb_window_t window = xcb_generate_id (connection);
    xcb_create_window (connection,
                       0,                             /* depth               */
                       window,
                       screen->root,                  /* parent window       */
                       0, 0,                          /* x, y                */
                       250, 150,                      /* width, height       */
                       10,                            /* border_width        */
                       XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                       screen->root_visual,           /* visual              */
                       value_mask, value_list);                     /* masks, not used     */


    /* set the title of the window */

    char *title = "Hello World !";
    xcb_change_property (connection,
                         XCB_PROP_MODE_REPLACE,
                         window,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         strlen (title),
                         title);


    /* set the title of the window icon */

    char *iconTitle = "Hello World ! (iconified)";
    xcb_change_property (connection,
                         XCB_PROP_MODE_REPLACE,
                         window,
                         XCB_ATOM_WM_ICON_NAME,
                         XCB_ATOM_STRING,
                         8,
                         strlen(iconTitle),
                         iconTitle);


    /* map the window on the screen */

    xcb_map_window (connection, window);
    xcb_flush (connection);


    /* event loop (in this case, no events to handle) */
    while(!finished && (event = xcb_wait_for_event(connection))) {
        switch(event->response_type) {
            case XCB_KEY_PRESS:
                btnPressed = ((xcb_key_press_event_t*)event);
                printf("\nKeycode: %d\n", btnPressed->detail);
                /* ESC */
                if (btnPressed->detail == 9) { // Finish if escape is pressed
                    finished = true;
                }
                break;
            case XCB_BUTTON_PRESS:
                printf("\nButton pressed: %u\n", ((xcb_button_press_event_t*)event)->detail);
                printf("X-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_x);
                printf("Y-coordinate: %u\n", ((xcb_button_press_event_t*)event)->event_y);
                break;
            case XCB_EXPOSE:
                break;
            default:
                printf("something else");
                break;
        }
        free(event);
    }

    //sleep(5); // Waits 5 seconds

    // Closing the connection to the X server
    printf("\nDisconnecting");
    xcb_disconnect(connection);

    return 0;
}