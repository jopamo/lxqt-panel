/* plugin-spacer/spacer.h
 * Spacer plugin implementation
 */

#ifndef SPACER_H
#define SPACER_H

#include "../panel/ioneg4panelplugin.h"
#include <QFrame>

class SpacerWidget : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString type READ getType)
  Q_PROPERTY(QString orientation READ getOrientation)

 public:
  const QString& getType() const throw() { return mType; }
  void setType(QString const& type);
  const QString& getOrientation() const throw() { return mOrientation; }
  void setOrientation(QString const& orientation);

 private:
  QString mType;
  QString mOrientation;
};

class Spacer : public QObject, public IOneG4PanelPlugin {
  Q_OBJECT

 public:
  Spacer(const IOneG4PanelPluginStartupInfo& startupInfo);

  virtual QWidget* widget() override { return &mSpacer; }
  virtual QString themeId() const override { return QStringLiteral("Spacer"); }

  bool isSeparate() const override { return true; }
  bool isExpandable() const override { return mExpandable; }

  virtual IOneG4PanelPlugin::Flags flags() const override { return HaveConfigDialog; }
  QDialog* configureDialog() override;

  virtual void realign() override;

 private slots:
  virtual void settingsChanged() override;

 private:
  void setSizes();

 private:
  SpacerWidget mSpacer;
  int mSize;
  bool mExpandable;
};

class SpacerPluginLibrary : public QObject, public IOneG4PanelPluginLibrary {
  Q_OBJECT
  // Q_PLUGIN_METADATA(IID "oneg4.org/Panel/PluginInterface/3.0")
  Q_INTERFACES(IOneG4PanelPluginLibrary)
 public:
  IOneG4PanelPlugin* instance(const IOneG4PanelPluginStartupInfo& startupInfo) const { return new Spacer(startupInfo); }
};

#endif