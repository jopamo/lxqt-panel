/* panel/ioneg4panel.h
 * Header file for ioneg4panel
 */

#ifndef IONEG4PANEL_H
#define IONEG4PANEL_H
#include <QRect>
#include "oneg4panelglobals.h"

class IOneG4PanelPlugin;
class QWidget;

/**
 **/
class ONEG4_PANEL_API IOneG4Panel {
 public:
  /**
   * @brief Specifies the position of the panel on screen.
   */
  enum Position {
    PositionBottom,  //!< The bottom side of the screen.
    PositionTop,     //!< The top side of the screen.
    PositionLeft,    //!< The left side of the screen.
    PositionRight    //!< The right side of the screen.
  };

  virtual ~IOneG4Panel() {}

  /**
   * @brief Returns the position of the panel. Possible values for the
   * return value are described by the Position enum.
   */
  virtual Position position() const = 0;

  /**
   * @brief Returns the edge length of the icons that are shown on the panel
   * in pixels. The icons are square.
   */
  virtual int iconSize() const = 0;
  /**
   * @brief Returns the number of lines/rows of this panel.
   */
  virtual int lineCount() const = 0;

  /**
   * @brief Helper function for convenient direction/alignment checking.
   * @return True if the panel is on the top or the bottom of the
   * screen; otherwise returns false.
   */
  bool isHorizontal() const { return position() == PositionBottom || position() == PositionTop; }

  /**
   * @brief Helper method that returns the global screen coordinates of the
   * panel, so you do not need to use QWidget::mapToGlobal() by yourself.
   * @return The QRect where the panel is located in global screen
   * coordinates.
   */
  virtual QRect globalGeometry() const = 0;

  /**
   * @brief Helper method for calculating the global screen position of a
   * popup window with size windowSize.
   * @param absolutePos Contains the global screen coordinates where the
   * popup should be appear, i.e. the point where the user has clicked.
   * @param windowSize The size that the window will occupy.
   * @return The global screen position where the popup window can be shown.
   */
  virtual QRect calculatePopupWindowPos(const QPoint& absolutePos, const QSize& windowSize) const = 0;
  /**
   * @brief Helper method for calculating the global screen position of a
   * popup window with size windowSize. The parameter plugin should be a
   * plugin
   * @param plugin Plugin that the popup window will belong to. The position
   * will be calculated according to the position of the plugin in the panel.
   * @param windowSize The size that the window will occupy.
   * @return The global screen position where the popup window can be shown.
   */
  virtual QRect calculatePopupWindowPos(const IOneG4PanelPlugin* plugin, const QSize& windowSize) const = 0;

  /*!
   * \brief By calling this function, a plugin (or any other object) notifies the panel
   * about showing a (standalone) window/menu -> the panel needs this to avoid "hiding" in case any
   * standalone window is shown. The widget/window must be shown later than this notification call because
   * the panel needs to observe its show/hide/close events.
   *
   * \param w the window that will be shown
   *
   */
  virtual void willShowWindow(QWidget* w) = 0;

  /*!
   * \brief By calling this function, a plugin notifies the panel about change of it's "static"
   * configuration
   *
   * \param plugin the changed plugin
   *
   * \sa IOneG4PanelPlugin::isSeparate(), IOneG4PanelPlugin::isExpandable
   */
  virtual void pluginFlagsChanged(const IOneG4PanelPlugin* plugin) = 0;

  /*!
   * \brief Checks if the panel is locked.
   */
  virtual bool isLocked() const = 0;
};

#endif  // IONEG4PANEL_H