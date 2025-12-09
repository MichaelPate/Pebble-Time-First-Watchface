#include <pebble.h>

// This main window is static, where all our UI elements will live
static Window *sMainWindow;
// The time is still text, it needs its own text layer
static TextLayer *sTimeTextLayer;

// Window load and unload methods handle creation and destuction of Window's sub-elements
static void mainWindow_load(Window *window)
{
  // First we need information on the display we are loading on
  Layer *windowLayer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(windowLayer);

  // Now we can apply those bounds to the text layers
  // The square displays are 52 px tall, round is 58, so we use the built in macro.
  // Since we only care about the Time 1 (basalt) for this face, technically we dont need to do this
  sTimeTextLayer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  // Set the colors, the text and font, and the alignment
  text_layer_set_background_color(sTimeTextLayer, GColorClear);
  text_layer_set_text_color(sTimeTextLayer, GColorBlack);
  text_layer_set_text(sTimeTextLayer, "00:00");
  text_layer_set_font(sTimeTextLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(sTimeTextLayer, GTextAlignmentCenter);

  // Finally, with our text layer setup, we can add it to the window
  layer_add_child(windowLayer, text_layer_get_layer(sTimeTextLayer));
}

static void mainWindow_unLoad(Window *window)
{
  // Must destroy the layer when we are done
  text_layer_destroy(sTimeTextLayer);
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