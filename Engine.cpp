/* MIT License
 * 
 * Copyright (c) 2026 Voronezh State University
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "Engine.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>


uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] = { 0 };

static Display* display = nullptr;
static Window window = None;
static int screen_num = 0;
static XVisualInfo* vinfo = nullptr;
static int visual_depth = 0;
static GC gc = nullptr;
static bool is_active = true;
static int cursor_x = 0;
static int cursor_y = 0;
static bool quited = false;
static XImage* ximg = nullptr;

static struct timeval tv_ref;

bool is_window_active()
{
  return is_active;
}

void clear_buffer()
{
  memset(buffer, 0, sizeof(buffer));
}

bool is_key_pressed(int button_vk_code)
{
  if (!is_active || !display)
    return false;

  // Map virtual key codes to X11 keysyms
  KeySym keysym = 0;

  switch (button_vk_code) {
    case VK_ESCAPE: keysym = XK_Escape; break;
    case VK_SPACE: keysym = XK_space; break;
    case VK_LEFT: keysym = XK_Left; break;
    case VK_UP: keysym = XK_Up; break;
    case VK_RIGHT: keysym = XK_Right; break;
    case VK_DOWN: keysym = XK_Down; break;
    case VK_RETURN: keysym = XK_Return; break;
    default:
      // Treat as ASCII character
      keysym = (KeySym)button_vk_code;
      break;
  }

  KeyCode keycode = XKeysymToKeycode(display, keysym);
  if (keycode == 0)
    return false;

  // Check with XQueryKeymap
  char keys[32];
  XQueryKeymap(display, keys);
  if (keys[keycode >> 3] & (1 << (keycode & 0x7))) {
    return true;
  }

  return false;
}

bool is_mouse_button_pressed(int mouse_button_index)
{
  if (!is_active || !display)
    return false;

  Window dummy_win = None;
  int dummy_x = 0, dummy_y = 0;
  unsigned int dummy_btn = 0;

  XQueryPointer(display, DefaultRootWindow(display),
                &dummy_win, &dummy_win,
                &dummy_x, &dummy_y, &cursor_x, &cursor_y, &dummy_btn);

  if (mouse_button_index == 0) {
    return (dummy_btn & Button1Mask) != 0;
  }

  if (mouse_button_index == 1) {
    return (dummy_btn & Button3Mask) != 0;
  }

  return false;
}

int get_cursor_x()
{
  return cursor_x;
}

int get_cursor_y()
{
  return cursor_y;
}

void schedule_quit_game()
{
  quited = true;
}

static float get_dt()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);

  long diff_ms = (tv.tv_sec - tv_ref.tv_sec) * 1000 + (tv.tv_usec - tv_ref.tv_usec) / 1000;
  if (diff_ms < 0) diff_ms = 0;

  tv_ref = tv;

  return (float)diff_ms / 1000.0f;
}

static void x11_init()
{
  display = XOpenDisplay(nullptr);
  if (!display) {
    fprintf(stderr, "Error: Could not open X display\n");
    exit(1);
  }

  screen_num = DefaultScreen(display);
  Window root = RootWindow(display, screen_num);

  // Set up visual and colormap
  XVisualInfo vinfo;
  XMatchVisualInfo(display, screen_num, 32, TrueColor, &vinfo);
  ::vinfo = new XVisualInfo(vinfo);
  ::visual_depth = vinfo.depth;

  XSetWindowAttributes attrs;
  attrs.colormap = XCreateColormap(display, root, vinfo.visual, AllocNone);
  attrs.background_pixmap = None;
  attrs.border_pixel = 0;
  attrs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     ExposureMask | FocusChangeMask | VisibilityChangeMask;

  window = XCreateWindow(
    display, root,
    0, 0,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    0,
    vinfo.depth,
    InputOutput,
    vinfo.visual,
    CWColormap | CWBorderPixel | CWEventMask,
    &attrs);

  if (!window) {
    fprintf(stderr, "Error: Could not create X window\n");
    XCloseDisplay(display);
    exit(1);
  }

  // Set window manager hints
  XSizeHints* hints = XAllocSizeHints();
  hints->flags = PMinSize | PMaxSize;
  hints->min_width = hints->max_width = SCREEN_WIDTH;
  hints->min_height = hints->max_height = SCREEN_HEIGHT;
  XSetWMNormalHints(display, window, hints);
  XFree(hints);

  // Set window title
  const char* title = "Asteroids";
  XTextProperty tp;
  char* str = const_cast<char*>(title);
  XStringListToTextProperty(&str, 1, &tp);
  XSetWMName(display, window, &tp);
  XSetWMIconName(display, window, &tp);
  XFree(tp.value);

  // Set WM_DELETE_WINDOW protocol
  Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);

  // Map window
  XMapWindow(display, window);

  // Create Graphics Context
  gc = XCreateGC(display, window, 0, nullptr);

  // Wait for MapNotify
  XEvent ev;
  bool mapped = false;
  while (!mapped) {
    XNextEvent(display, &ev);
    if (ev.type == MapNotify) {
      mapped = true;
    }
  }

  // Initialize cursor position
  {
    Window dummy_win = None;
    int dummy_x = 0, dummy_y = 0;
    unsigned int dummy_btn = 0;
    XQueryPointer(display, window, &dummy_win, &dummy_win, &dummy_x, &dummy_y, &cursor_x, &cursor_y, &dummy_btn);
  }

  // Reference time
  gettimeofday(&tv_ref, nullptr);

  // Create XImage for rendering
  ximg = XCreateImage(
    display,
    ::vinfo->visual,
    ::visual_depth,
    ZPixmap,
    0,
    (char*)buffer,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    32,
    SCREEN_WIDTH * 4);

  is_active = true;
}

static void x11_cleanup()
{
  if (display) {
    if (ximg) {
      XDestroyImage(ximg);
      ximg = nullptr;
    }
    if (window) {
      XDestroyWindow(display, window);
      window = None;
    }
    if (::vinfo) {
      delete ::vinfo;
      ::vinfo = nullptr;
    }
    XCloseDisplay(display);
    display = nullptr;
  }
}

static void x11_render()
{
  if (!display || !window || !gc || !ximg)
    return;

  XPutImage(display, window, gc, ximg, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  XFlush(display);
}

int main(int, char**)
{
  x11_init();

  initialize();

  float dt = 0.0f;
  XEvent ev;
  bool running = true;

  while (running && !quited) {
    // Process X events
    if (XPending(display) > 0) {
      XNextEvent(display, &ev);

      switch (ev.type) {
        case KeyPress:
        case KeyRelease:
          break;
        case ButtonPress:
        case ButtonRelease:
          break;
        case MotionNotify:
          cursor_x = ev.xmotion.x;
          cursor_y = ev.xmotion.y;
          break;
        case Expose:
          while (XPending(display)) {
            XNextEvent(display, &ev);
          }
          break;
        case FocusIn:
          is_active = true;
          break;
        case FocusOut:
          is_active = false;
          break;
        case VisibilityNotify:
          is_active = (ev.xvisibility.state != VisibilityFullyObscured);
          break;
        case DestroyNotify:
          quited = true;
          break;
        case ClientMessage: {
          Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
          if (ev.xclient.data.l[0] == (long)wm_delete) {
            quited = true;
          }
          break;
        }
        default:
          break;
      }
    }

    is_active = is_window_active();


    if (is_active) {
      dt = get_dt();
      if (dt > 0.1f)
        dt = 0.1f;

      act(dt);

      if (!quited) {
        draw();
        x11_render();
      }
    }

    usleep(1000); // 1ms sleep to reduce CPU usage
  }

  finalize();
  x11_cleanup();

  return 0;
}
