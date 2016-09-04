#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct {
  int32_t date_format_index;
  int32_t day_format_index;
  char full_date_format[15];
} Settings;

extern Settings settings;

void settings_init();
void settings_save();

void settings_set_date_format_index(int32_t);
void settings_set_day_format_index(int32_t);

#endif /* SETTINGS_H */
