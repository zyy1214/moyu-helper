#include "colorprovider.h"

ColorProvider ColorProvider::color_provider;

ColorProvider::ColorProvider() {
    register_color("selected", "#C0DCF3", "#7FB5E1");
    register_color("text_color", "#000000", "#DDDDDD");
    register_color("point_positive", "#1770E4", "#31AFFF");
    register_color("point_negative", "#CC0000", "#D00000");
    register_color("gray", "#606060", "#D0D0D0");
    register_color("palette", "#FFFFFF", "#000000");
}

void ColorProvider::register_color(QString color_name, QString light_mode_color, QString dark_mode_color) {
    light_mode_colors[color_name] = QColor(light_mode_color);
    dark_mode_colors[color_name] = QColor(dark_mode_color);
}

QColor ColorProvider::get_color(QString color_name) {
    if (dark_mode) {
        return dark_mode_colors[color_name];
    } else {
        return light_mode_colors[color_name];
    }
}

QColor get_color(QString color_name) {
    return ColorProvider::get_color_provider().get_color(color_name);
}
