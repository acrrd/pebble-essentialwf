#include <pebble.h>

#include "settings.h"

static const int32_t STORAGE_CURRENT_VERSION = 0;

static const uint32_t STORAGE_VERSION_KEY = 0;
static const uint32_t DATE_FORMAT_KEY = 1;

Settings settings;

void settings_load_defaults() {
  settings.date_format_index = 0;
}

void settings_save() {
  persist_write_int(STORAGE_VERSION_KEY, STORAGE_CURRENT_VERSION);
  persist_write_int(DATE_FORMAT_KEY, settings.date_format_index);
}

void settings_init() {
  settings_load_defaults();

  if (!persist_exists(STORAGE_VERSION_KEY)) {
    settings_save();
  }

  settings_set_date_format_index(persist_read_int(DATE_FORMAT_KEY));
}

void settings_set_date_format_index(int32_t i) {
  if (i >= 0 && i < 3)
    settings.date_format_index = i;
}
