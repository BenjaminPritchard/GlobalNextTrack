// globalHotKey
// Execute python websocket server
// Listens for global hotkeys
// When a global hotkey is pressed, send a message to the websocket server

// this code mostly comes from here: https://github.com/anko/xkbcat/blob/master/xkbcat.c

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

extern char **environ;

int xiOpcode;
Display *disp;
FILE *pipe_fp;

#define VERSION "1.0"

bool initKB()
{

    // Connect to X display
    // this is just harded for now...
    disp = XOpenDisplay(":0");
    if (NULL == disp)
    {
        fprintf(stderr, "Cannot open X display: %s\n", ":0");
        return false;
    }

    { // Test for XInput 2 extension
        int queryEvent, queryError;
        if (!XQueryExtension(disp, "XInputExtension", &xiOpcode,
                             &queryEvent, &queryError))
        {
            fprintf(stderr, "X Input extension not available\n");
            exit(2);
        }
    }
    { // Request XInput 2.0, to guard against changes in future versions
        int major = 2, minor = 0;
        int queryResult = XIQueryVersion(disp, &major, &minor);
        if (queryResult == BadRequest)
        {
            fprintf(stderr, "Need XI 2.0 support (got %d.%d)\n", major, minor);
            exit(3);
        }
        else if (queryResult != Success)
        {
            fprintf(stderr, "XIQueryVersion failed!\n");
            exit(4);
        }
    }
    { // Register to receive XInput events
        Window root = DefaultRootWindow(disp);
        XIEventMask m;
        m.deviceid = XIAllMasterDevices;
        m.mask_len = XIMaskLen(XI_LASTEVENT);
        m.mask = calloc(m.mask_len, sizeof(char));
        XISetMask(m.mask, XI_RawKeyPress);
        XISelectEvents(disp, root, &m, 1 /*number of masks*/);
        XSync(disp, false);
        free(m.mask);
    }
    return true;
}

void CheckForHotKeys()
{
    while (1)
    {
        XEvent event;
        XGenericEventCookie *cookie = (XGenericEventCookie *)&event.xcookie;

        XNextEvent(disp, &event);

        if (XGetEventData(disp, cookie) &&
            cookie->type == GenericEvent &&
            cookie->extension == xiOpcode)
        {
            switch (cookie->evtype)
            {
            case XI_RawKeyRelease:
            case XI_RawKeyPress:
            {
                XIRawEvent *ev = cookie->data;

                // Ask X what it calls that key; skip if it doesn't know
                KeySym s = XkbKeycodeToKeysym(disp, ev->detail,
                                              0 /*group*/, 0 /*shift level*/);
                if (NoSymbol == s)
                    continue;
                char *str = XKeysymToString(s);
                if (NULL == str)
                    continue;

                if (strcmp(str, "F3") == 0)
                {
                    fputs("next\n", pipe_fp);
                    fflush(pipe_fp);
                }
                else if (strcmp(str, "F4") == 0)
                {
                    fputs("prev\n", pipe_fp);
                    fflush(pipe_fp);
                }

                break;
            }
            }
        }
    }
}

void parseCmdLine(int argc, char **argv)
{
    bool printHelp;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0)
            {
                printf(
                    "globalHotKey, written by Benjamin Pritchard.\n"
                    "\n"
                    "Description goes here... \n"
                    "\n"
                    "Usage: globalHotKey [OPTIONS]\n"
                    "   -h, --help                  Displays this information.\n"
                    "   -v, --version               Displays version information\n"
                    "\n"
                    "Source code at: https://github.com/BenjaminPritchard/VanGoghFlowGTK3\n"
                    "\n");
                exit(0);
            }
            else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verson") == 0)
            {
                printf("globalKeyHot version %s\n", VERSION);
                exit(0);
            }
            else
            {
                fprintf(stderr, "Error: unknown option %s\n", argv[i]);
                exit(1);
            }
        }
    }
}

int main(int argc, char **argv)
{

    if ((pipe_fp = popen("/usr/bin/python3 ws.py", "w")) == NULL)
    {
        perror("popen");
        exit(1);
    }

    //pclose(pipe_fp);

    initKB();
    CheckForHotKeys();
}
