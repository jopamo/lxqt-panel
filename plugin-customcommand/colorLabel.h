/* plugin-customcommand/colorLabel.h
 * Header file for colorLabel
 */

#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class ColorLabel : public QLabel {
  Q_OBJECT

 public:
  explicit ColorLabel(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~ColorLabel();

  void setColor(const QColor& color, bool announceChange = false);
  QColor getColor() const;

  void reset();

 signals:
  void colorChanged();

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  QColor color_;
};

#endif  // COLORLABEL_H