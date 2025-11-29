/* plugin-sysstat/lxqtsysstatcolours.h
 * Header file for lxqtsysstatcolours
 */

#ifndef LXQTSYSSTATCOLOURS_HPP
#define LXQTSYSSTATCOLOURS_HPP

#include <QDialog>

#include <QMap>
#include <QString>
#include <QColor>

namespace Ui {
class LXQtSysStatColours;
}

class QSignalMapper;
class QAbstractButton;
class QPushButton;

class LXQtSysStatColours : public QDialog {
  Q_OBJECT

 public:
  explicit LXQtSysStatColours(QWidget* parent = nullptr);
  ~LXQtSysStatColours();

  typedef QMap<QString, QColor> Colours;

  void setColours(const Colours&);

  Colours colours() const;

  Colours defaultColours() const;

 signals:
  void coloursChanged();

 public slots:
  void on_buttons_clicked(QAbstractButton*);

  void selectColour(const QString&);

  void restoreDefaults();
  void reset();
  void apply();

 private:
  Ui::LXQtSysStatColours* ui;

  QSignalMapper* mSelectColourMapper;
  QMap<QString, QPushButton*> mShowColourMap;

  Colours mDefaultColours;
  Colours mInitialColours;
  Colours mColours;

  void applyColoursToButtons();
};

#endif  // LXQTSYSSTATCOLOURS_HPP