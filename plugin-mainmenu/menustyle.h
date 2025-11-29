/* plugin-mainmenu/menustyle.h
 * Main menu plugin implementation
 */

#ifndef MENUSTYLE_H
#define MENUSTYLE_H

#include <QProxyStyle>

class MenuStyle : public QProxyStyle {
  Q_OBJECT
 public:
  // reserved value which gets the icon size from the parent style
  static constexpr int DEFAULT_ICON_SIZE = -1;

  explicit MenuStyle();
  int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;
  int styleHint(StyleHint hint,
                const QStyleOption* option = 0,
                const QWidget* widget = 0,
                QStyleHintReturn* returnData = 0) const;
  int iconSize() const { return mIconSize; }
  void setIconSize(int value) { mIconSize = value; }

 private:
  int mIconSize;
};

#endif  // MENUSTYLE_H