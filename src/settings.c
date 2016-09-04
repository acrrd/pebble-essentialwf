#include <pebble.h>

#include "settings.h"

static const int32_t STORAGE_CURRENT_VERSION = 0;

static const uint32_t STORAGE_VERSION_KEY = 0;
static const uint32_t DATE_FORMAT_KEY = 1;
static const uint32_t DAY_FORMAT_KEY = 2;

Settings settings;

static char date_formats[][12] = {"%Y/%m/%d", "%m/%d/%Y", "%d/%m/%Y"};

static char day_formats[][7] = {"%%a %s", "%s %%a", "%s"};

void settings_set_full_date_format();

void settings_load_defaults() {
  settings.date_format_index = 0;
  settings.day_format_index = 0;
  settings_set_full_date_format();
}

void settings_save() {
  persist_write_int(STORAGE_VERSION_KEY, STORAGE_CURRENT_VERSION);
  persist_write_int(DATE_FORMAT_KEY, settings.date_format_index);
  persist_write_int(DAY_FORMAT_KEY, settings.day_format_index);
}

void settings_init() {
  settings_load_defaults();

  if (!persist_exists(STORAGE_VERSION_KEY)) {
    settings_save();
  }

  settings_set_date_format_index(persist_read_int(DATE_FORMAT_KEY));
  settings_set_day_format_index(persist_read_int(DAY_FORMAT_KEY));
}

void settings_set_date_format_index(int32_t i) {
  if (i >= 0 && i < 3) {
    settings.date_format_index = i;
    settings_set_full_date_format();
  }
}

void settings_set_day_format_index(int32_t i) {
  if (i >= 0 && i < 3) {
    settings.day_format_index = i;
    settings_set_full_date_format();
  }
}

void settings_set_full_date_format() {
  static int8_t date_index = -1;
  static int8_t day_index = -1;

  if (date_index != settings.date_format_index ||
      day_index != settings.day_format_index) {
    snprintf(settings.full_date_format, sizeof(settings.full_date_format),
             day_formats[settings.day_format_index],
             date_formats[settings.date_format_index]);
    date_index = settings.date_format_index;
    day_index = settings.day_format_index;
  }
}
