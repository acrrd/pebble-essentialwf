#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct { uint32_t date_format_index; } Settings;

extern Settings settings;

void settings_init();
void settings_save();

void settings_set_date_format_index(int32_t);

#endif /* SETTINGS_H */
