#include <stdio.h>
#include <xcb/xcb.h>
#include <inttypes.h>

int main () {
    /* https://xcb.freedesktop.org/tutorial/ */
    /* Open the connection to the X server. Use the DISPLAY environment variable */
    int i, screenNum;
    xcb_connection_t *connection = xcb_connect (NULL, &screenNum);


    /* Get the screen whose number is screenNum */

    const xcb_setup_t *setup = xcb_get_setup (connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);

    // we want the screen at index screenNum of the iterator
    for (i = 0; i < screenNum; ++i) {
        xcb_screen_next (&iter);
    }

    xcb_screen_t *screen = iter.data;


    /* report */

    printf ("\n");
    printf ("Informations of screen %"PRIu32":\n", screen->root);
    printf ("  width.........: %"PRIu16"\n", screen->width_in_pixels);
    printf ("  height........: %"PRIu16"\n", screen->height_in_pixels);
    printf ("  white pixel...: %"PRIu32"\n", screen->white_pixel);
    printf ("  black pixel...: %"PRIu32"\n", screen->black_pixel);
    printf ("\n");

    return 0;
}

/*
 * x.org's way
int
main ()
{
  xcb_connection_t     *c;
  xcb_screen_t         *screen;
  int                   screen_nbr;
  xcb_screen_iterator_t iter;

  // Open the connection to the X server. Use the DISPLAY environment variable
c = xcb_connect (NULL, &screen_nbr);

// Get the screen #screen_nbr
iter = xcb_setup_roots_iterator (xcb_get_setup (c));
for (; iter.rem; --screen_nbr, xcb_screen_next (&iter))
if (screen_nbr == 0) {
screen = iter.data;
break;
}

printf ("\n");
printf ("Informations of screen %ld:\n", screen->root);
printf ("  width.........: %d\n", screen->width_in_pixels);
printf ("  height........: %d\n", screen->height_in_pixels);
printf ("  white pixel...: %ld\n", screen->white_pixel);
printf ("  black pixel...: %ld\n", screen->black_pixel);
printf ("\n");

return 0;
}
 */