#ifndef ONEG4_CONFIG_DIALOG_H
#define ONEG4_CONFIG_DIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVector>

namespace OneG4 {

class Settings;

class ConfigDialog : public QDialog {
  Q_OBJECT

 public:
  ConfigDialog(const QString& title, Settings* settings, QWidget* parent = nullptr);

  void addPage(QWidget* page, const QString& title, const QString& iconName);
  void showPage(QWidget* page);
  void updateIconThemeSettings();

 signals:
  void reset();

 private slots:
  void handleButton(QAbstractButton* button);

 private:
  struct PageInfo {
    QWidget* widget;
    QString title;
    QString iconName;
  };

  QVector<PageInfo> mPages;
  QTabWidget* mTabWidget;
  QDialogButtonBox* mButtonBox;
};

}  // namespace OneG4

#endif  // ONEG4_CONFIG_DIALOG_H
