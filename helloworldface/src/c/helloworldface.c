#include <pebble.h>

// This main window is static, where all our UI elements will live
static Window *sMainWindow;

// Window load and unload methods handle creation and destuction of Window's sub-elements
static void mainWindow_load(Window *window)
{

}

static void mainWindow_unLoad(Window *window)
{

}

static void init()
{
  // Create our main window, assign its handlers, and display it
  sMainWindow = window_create();
  window_set_window_handlers(sMainWindow, (WindowHandlers) {
    .load = mainWindow_load,
    .unload = mainWindow_unLoad
  });
  window_stack_push(sMainWindow, true); // true, for sliding animation
}

static void deinit()
{
  // Dont forget to destroy the window object when we are done
  window_destroy(sMainWindow);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}