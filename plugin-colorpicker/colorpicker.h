/* plugin-colorpicker/colorpicker.h
 * Header file for colorpicker
 */

#ifndef LXQT_COLORPICKER_H
#define LXQT_COLORPICKER_H

#include "../panel/ilxqtpanelplugin.h"
#include <QFrame>
#include <QToolButton>

class QMenu;
class ColorButton;

class ColorPickerWidget : public QWidget {
  Q_OBJECT

 public:
  ColorPickerWidget(QWidget* parent = nullptr);
  ~ColorPickerWidget();

  QMenu* popupMenu() { return mColorsMenu; }
  QToolButton* pickerButton() { return mPickerButton; }
  ColorButton* colorButton() { return mColorButton; }
  void update(bool isHorizontal);

 signals:

  void showMenuRequested(QMenu* menu);

 protected:
  void mouseReleaseEvent(QMouseEvent* event);

 private slots:
  void captureMouse();

 private:
  static const QString svgIcon;

  QMenu* mColorsMenu{nullptr};
  QToolButton* mPickerButton{nullptr};
  ColorButton* mColorButton{nullptr};
  QAction* mClearListAction{nullptr};
  QFrame* mSeparator{nullptr};
  bool mCapturing{false};
  QList<QColor> mColorsList{};

  QIcon colorIcon(QColor color);
  void buildMenu();
  void paste(const QString color) const;
};

class ColorPicker : public QObject, public ILXQtPanelPlugin {
  Q_OBJECT
 public:
  ColorPicker(const ILXQtPanelPluginStartupInfo& startupInfo);
  ~ColorPicker();

  virtual QWidget* widget() override { return &mWidget; }
  virtual QString themeId() const override { return QStringLiteral("ColorPicker"); }

  virtual bool isSeparate() const override { return true; }

  virtual void realign() override;

 private:
  ColorPickerWidget mWidget;
};

class ColorPickerLibrary : public QObject, public ILXQtPanelPluginLibrary {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(ILXQtPanelPluginLibrary)
 public:
  ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo& startupInfo) const {
    return new ColorPicker(startupInfo);
  }
};

#endif