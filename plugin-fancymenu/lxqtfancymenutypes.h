/* plugin-fancymenu/lxqtfancymenutypes.h
 * Fancy menu plugin implementation
 */

#ifndef LXQTFANCYMENUTYPES_H
#define LXQTFANCYMENUTYPES_H

#include <qnamespace.h>

enum LXQtFancyMenuButtonPosition : bool { Bottom = 0, Top = 1 };

enum LXQtFancyMenuCategoryPosition : bool { Left = 0, Right = 1 };

enum class LXQtFancyMenuItemType { AppItem = 0, CategoryItem, SeparatorItem };

static constexpr const int LXQtFancyMenuItemIsSeparatorRole = Qt::UserRole + 1;

#endif  // LXQTFANCYMENUTYPES_H