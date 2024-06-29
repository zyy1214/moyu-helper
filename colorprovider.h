#ifndef COLORPROVIDER_H
#define COLORPROVIDER_H

#include <map>

#include <QColor>
#include <QObject>
#include <QString>

class ColorProvider : public QObject
{
    Q_OBJECT
    bool dark_mode = false;
    std::map<QString, QColor> light_mode_colors;
    std::map<QString, QColor> dark_mode_colors;
    ColorProvider();
    static ColorProvider color_provider;
public:
    void register_color(QString color_name, QString light_mode_color, QString dark_mode_color);
    QColor get_color(QString color_name);
    static ColorProvider &get_color_provider() {
        return color_provider;
    }
    static void set_dark_mode(bool dark_mode) {
        get_color_provider().dark_mode = dark_mode;
        emit get_color_provider().color_mode_switched();
    }

signals:
    void color_mode_switched();
};

QColor get_color(QString color_name);

#endif // COLORPROVIDER_H
