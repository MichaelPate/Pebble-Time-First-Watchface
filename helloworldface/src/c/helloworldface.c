#include <pebble.h>

// This main window is static, where all our UI elements will live
static Window *sMainWindow;
// The time is still text, it needs its own text layer
static TextLayer *sTimeTextLayer;
// This will be the font we imported in part 2 of the watchface tutorial
static GFont sTimeFont;

static Layer *bgColorsLayer;
static int divider_x = 55;  // where is the vertical line down the face

static void updateBgColors_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  int leftTileWidth = 55;
  //GRect leftTile = GRect(0, PBL_IF_ROUND_ELSE(58,52), leftTileWidth, 50);
  //GRect rightTile = GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w - leftTileWidth, 50);
  GRect leftTile = GRect(0, 0, divider_x, bounds.size.h);
  GRect rightTile = GRect(divider_x, 0, bounds.size.w - divider_x, bounds.size.h);
  
  graphics_context_set_fill_color(ctx, GColorRed);
  graphics_fill_rect(ctx, leftTile, 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorBlue);
  graphics_fill_rect(ctx, rightTile, 0, GCornerNone);

}

// Window load and unload methods handle creation and destuction of Window's sub-elements
static void mainWindow_load(Window *window)
{
  // First we need information on the display we are loading on
  Layer *windowLayer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(windowLayer);

  // We want to add some colored boxes to the background
  bgColorsLayer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(bgColorsLayer, updateBgColors_proc);
  layer_add_child(windowLayer, bgColorsLayer);

  // Now we can apply those bounds to the text layers
  // The square displays are 52 px tall, round is 58, so we use the built in macro.
  // Since we only care about the Time 1 (basalt) for this face, technically we dont need to do this
  sTimeTextLayer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  // Set the colors, the text and font, and the alignment
  text_layer_set_background_color(sTimeTextLayer, GColorClear);
  text_layer_set_text_color(sTimeTextLayer, GColorBlack);
  //text_layer_set_text(sTimeTextLayer, "00:00"); //we are showing the time here, dont need this placeholder
  //text_layer_set_font(sTimeTextLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD)); // we are now using our imported font
  sTimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  text_layer_set_font(sTimeTextLayer, sTimeFont);
  text_layer_set_text_alignment(sTimeTextLayer, GTextAlignmentCenter);

  // Finally, with our text layer setup, we can add it to the window
  layer_add_child(windowLayer, text_layer_get_layer(sTimeTextLayer));
}

static void mainWindow_unLoad(Window *window)
{
  // Must destroy the layer when we are done
  text_layer_destroy(sTimeTextLayer);

  // Also unload the font
  fonts_unload_custom_font(sTimeFont);
}

static void updateTime()
{
  // Grab our tm stricture from localtime()
  time_t temp = time(NULL); // reserve a spot for the time to live
  struct tm *tickTime = localtime(&temp);

  // Get the hours and minutes and write to a buffer
  static char sBuffer[8];
  strftime(sBuffer, sizeof(sBuffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tickTime);

  // Now we can add to the time text layer
  text_layer_set_text(sTimeTextLayer, sBuffer);
}

// Whenever the tickHandler is called, we are given a struct with the time and the units changed
static void tickHandler(struct tm *tick_time, TimeUnits unitsChanged)
{
  // We can do other stuff here but for now just update the time
  updateTime();
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

  // The updateTime method will be called every minute but lets call it now
  // so its showing the time as soon as the face loads instead of the next minute
  updateTime();

  // We need to subscribe to the tick timer to get the time
  // We will call the tickHandler method above every minute
  tick_timer_service_subscribe(MINUTE_UNIT, tickHandler);
}

static void deinit()
{
  // Dont forget to destroy the window object when we are done
  window_destroy(sMainWindow);

  layer_destroy(bgColorsLayer);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}