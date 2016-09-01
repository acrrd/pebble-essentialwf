#include <pebble.h>

static Window* s_main_window;
static TextLayer* s_time_layer;
static TextLayer* s_time_pm_layer;
static TextLayer* s_date_layer;
static TextLayer* s_battery_layer;
static BitmapLayer* s_bt_icon_layer;
static GBitmap* s_bt_icon_bitmap;

static int8_t date_format_conf;
static char date_formats[][15] = {"%a %Y/%m/%d", "%a %m/%d/%Y", "%a %d/%m/%Y"};
static char* date_format = date_formats[0];

static void update_time() {
  time_t temp = time(NULL);
  struct tm* tick_time = localtime(&temp);

  static char format24_buffer[6] = "%H:%M";
  static char format12_buffer[6] = "%I:%M";
  static char* format_buffer = format24_buffer;

  if (clock_is_24h_style()) {
    format_buffer = format24_buffer;
    layer_set_hidden(text_layer_get_layer(s_time_pm_layer), true);
  } else {
    format_buffer = format12_buffer;
    if (tick_time->tm_hour > 11) {
      text_layer_set_text(s_time_pm_layer, "PM");
    } else {
      text_layer_set_text(s_time_pm_layer, "AM");
    }
    layer_set_hidden(text_layer_get_layer(s_time_pm_layer), false);
  }

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), format_buffer, tick_time);
  text_layer_set_text(s_time_layer, s_buffer);

  static char date_buffer[15];
  strftime(date_buffer, sizeof(date_buffer), date_format, tick_time);
  text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm* tick_time, TimeUnits units_changed) {
  update_time();
}

static void battery_callback(BatteryChargeState state) {
  static char battery_buffer[6];

  snprintf(battery_buffer, sizeof(battery_buffer), "%i%%",
           state.charge_percent);
  text_layer_set_text(s_battery_layer, battery_buffer);
}

static void bluetooth_callback(bool connected) {
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);
}

static void inbox_received_handler(DictionaryIterator* iter, void* context) {
  Tuple* date_format_conf = dict_find(iter, MESSAGE_KEY_DATE_FORMAT);
  if (date_format_conf) {
    int8_t date_format_index = date_format_conf->value->int8;
    if (date_format_index >= 0 && date_format_index < 3) {
      date_format = date_formats[date_format_index];
      update_time();
    }
  }
}

static void main_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Time
  s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer,
                      fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text(s_time_layer, "00:00");

  // Compute vertical alignment
  Layer* time_layer = text_layer_get_layer(s_time_layer);
  GRect time_frame = layer_get_frame(time_layer);
  GSize time_content_size = text_layer_get_content_size(s_time_layer);
  int time_x = time_frame.origin.x;
  int time_y =
      time_frame.origin.y + (time_frame.size.h - time_content_size.h) / 2;
  time_y -= 20;
  time_frame = GRect(time_x, time_y, time_frame.size.w, time_content_size.h);
  layer_set_frame(time_layer, time_frame);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Time AM/PM
  s_time_pm_layer = text_layer_create(
      GRect(bounds.size.w - PBL_IF_ROUND_ELSE(43, 25), time_frame.origin.y - 5,
            bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_time_pm_layer, GColorClear);
  text_layer_set_text_color(s_time_pm_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_pm_layer, GTextAlignmentLeft);
  text_layer_set_font(s_time_pm_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  layer_add_child(window_layer, text_layer_get_layer(s_time_pm_layer));

  // Date
  s_date_layer =
      text_layer_create(GRect(0, time_frame.origin.y + time_frame.size.h,
                              bounds.size.w, bounds.size.h));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer,
                      fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));

  // Battery
  s_battery_layer =
      text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_font(s_battery_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  text_layer_set_text(s_battery_layer, "100%");

  Layer* battery_layer = text_layer_get_layer(s_battery_layer);
  GRect battery_frame = layer_get_frame(battery_layer);
  GSize battery_content_size = text_layer_get_content_size(s_battery_layer);
  int battery_x =
      bounds.size.w - battery_content_size.w - PBL_IF_ROUND_ELSE(34, 0);
  int battery_y = PBL_IF_ROUND_ELSE(12, 0);
  battery_frame = GRect(battery_x, battery_y, battery_content_size.w,
                        battery_content_size.h);
  layer_set_frame(battery_layer, battery_frame);

  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

  // BT
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NOBT_ICON);
  s_bt_icon_layer = bitmap_layer_create(
      GRect(PBL_IF_ROUND_ELSE(35, 0), PBL_IF_ROUND_ELSE(18, 5), 13, 13));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);

  layer_add_child(window_get_root_layer(window),
                  bitmap_layer_get_layer(s_bt_icon_layer));
}

static void main_window_unload(Window* window) {
  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
}

static void init() {
  setlocale(LC_TIME, "");

  s_main_window = window_create();

  window_set_background_color(s_main_window, GColorBlack);

  window_set_window_handlers(
      s_main_window,
      (WindowHandlers){.load = main_window_load, .unload = main_window_unload});

  window_stack_push(s_main_window, true);

  // Time
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  update_time();

  // Battery
  battery_state_service_subscribe(battery_callback);

  battery_callback(battery_state_service_peek());

  // BT
  connection_service_subscribe((ConnectionHandlers){
      .pebble_app_connection_handler = bluetooth_callback});

  bluetooth_callback(connection_service_peek_pebble_app_connection());

  // Init AppMessage
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);
}

static void deinit() {
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();

  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
