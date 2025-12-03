/* plugin-worldclock/oneg4worldclock.cpp
 * Implementation file for oneg4worldclock
 */

#include "oneg4worldclock.h"

#include <OneG4/Globals.h>

#include <QCalendarWidget>
#include <QDate>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QLocale>
#include <QScopedArrayPointer>
#include <QTimer>
#include <QWheelEvent>
#include <QToolTip>

OneG4WorldClock::OneG4WorldClock(const IOneG4PanelPluginStartupInfo& startupInfo)
    : QObject(),
      IOneG4PanelPlugin(startupInfo),
      mPopup(nullptr),
      mTimer(new QTimer(this)),
      mUpdateInterval(1),
      mTimeZoneWheel(true),
      mAutoRotate(true),
      mShowWeekNumber(true),
      mShowTooltip(false),
      mPopupContent(nullptr) {
  mMainWidget = new QWidget();
  mMainWidget->installEventFilter(this);
  mContent = new ActiveLabel();
  mRotatedWidget = new OneG4::RotatedWidget(*mContent, mMainWidget);

  QVBoxLayout* borderLayout = new QVBoxLayout(mMainWidget);
  borderLayout->setContentsMargins(0, 0, 0, 0);
  borderLayout->setSpacing(0);
  borderLayout->addWidget(mRotatedWidget, 0, Qt::AlignCenter);

  mContent->setObjectName(QLatin1String("WorldClockContent"));

  mContent->setAlignment(Qt::AlignCenter);

  settingsChanged();

  mTimer->setTimerType(Qt::PreciseTimer);
  connect(mTimer, &QTimer::timeout, this, &OneG4WorldClock::timeout);

  connect(mContent, &ActiveLabel::wheelScrolled, this, &OneG4WorldClock::wheelScrolled);
}

OneG4WorldClock::~OneG4WorldClock() {
  delete mMainWidget;
}

void OneG4WorldClock::timeout() {
  if (QTime::currentTime().msec() > 500)
    restartTimer();
  updateTimeText();
}

void OneG4WorldClock::updateTimeText() {
  QDateTime now = QDateTime::currentDateTime();
  QString timeZoneName = mActiveTimeZone;
  if (timeZoneName == QLatin1String("local"))
    timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());
  QTimeZone timeZone(timeZoneName.toLatin1());
  QDateTime tzNow = now.toTimeZone(timeZone);

  bool isUpToDate(true);
  if (!mShownTime.isValid())  // first time or forced update
  {
    isUpToDate = false;
    if (mUpdateInterval < 60000)
      mShownTime = tzNow.addMSecs(-tzNow.time().msec());  // s
    else if (mUpdateInterval < 3600000)
      mShownTime = tzNow.addSecs(-tzNow.time().second());  // m
    else
      mShownTime = tzNow.addSecs(-tzNow.time().minute() * 60 - tzNow.time().second());  // h
  }
  else {
    qint64 diff = mShownTime.secsTo(tzNow);
    if (mUpdateInterval < 60000) {
      if (diff < 0 || diff >= 1) {
        isUpToDate = false;
        mShownTime = tzNow.addMSecs(-tzNow.time().msec());
      }
    }
    else if (mUpdateInterval < 3600000) {
      if (diff < 0 || diff >= 60) {
        isUpToDate = false;
        mShownTime = tzNow.addSecs(-tzNow.time().second());
      }
    }
    else if (diff < 0 || diff >= 3600) {
      isUpToDate = false;
      mShownTime = tzNow.addSecs(-tzNow.time().minute() * 60 - tzNow.time().second());
    }
  }
  if (!isUpToDate) {
    const QSize old_size = mContent->sizeHint();
    mContent->setText(QLocale::system().toString(tzNow, preformat(mFormat, timeZone, tzNow)));
    if (old_size != mContent->sizeHint())
      mRotatedWidget->adjustContentSize();
    mRotatedWidget->update();
    updatePopupContent();
  }
}

void OneG4WorldClock::setTimeText() {
  mShownTime = QDateTime();  // force an update
  updateTimeText();
}

void OneG4WorldClock::restartTimer() {
  mTimer->stop();
  // check the time every second even if the clock doesn't show seconds
  // because otherwise, the shown time might be vey wrong after resume
  mTimer->setInterval(1000);

  int delay = static_cast<int>(1000 - (static_cast<long long>(QTime::currentTime().msecsSinceStartOfDay()) % 1000));
  QTimer::singleShot(delay, Qt::PreciseTimer, this, &OneG4WorldClock::updateTimeText);
  QTimer::singleShot(delay, Qt::PreciseTimer, mTimer, SLOT(start()));
}

void OneG4WorldClock::settingsChanged() {
  static const QRegularExpression regexp(QLatin1String("'[^']*'"));

  PluginSettings* _settings = settings();

  QString oldFormat = mFormat;

  mTimeZones.clear();

  const QList<QMap<QString, QVariant> > array = _settings->readArray(QLatin1String("timeZones"));
  for (const auto& map : array) {
    QString timeZoneName = map.value(QLatin1String("timeZone"), QString()).toString();
    mTimeZones.append(timeZoneName);
    mTimeZoneCustomNames[timeZoneName] = map.value(QLatin1String("customName"), QString()).toString();
  }

  if (mTimeZones.isEmpty())
    mTimeZones.append(QLatin1String("local"));

  mDefaultTimeZone = _settings->value(QLatin1String("defaultTimeZone"), QString()).toString();
  if (mDefaultTimeZone.isEmpty())
    mDefaultTimeZone = mTimeZones[0];
  mActiveTimeZone = mDefaultTimeZone;

  bool longTimeFormatSelected = false;

  QString formatType = _settings->value(QLatin1String("formatType"), QString()).toString();
  QString dateFormatType = _settings->value(QLatin1String("dateFormatType"), QString()).toString();
  bool advancedManual = _settings->value(QLatin1String("useAdvancedManualFormat"), false).toBool();

  // backward compatibility
  if (formatType == QLatin1String("custom")) {
    formatType = QLatin1String("short-timeonly");
    dateFormatType = QLatin1String("short");
    advancedManual = true;
  }
  else if (formatType == QLatin1String("short")) {
    formatType = QLatin1String("short-timeonly");
    dateFormatType = QLatin1String("short");
    advancedManual = false;
  }
  else if ((formatType == QLatin1String("full")) || (formatType == QLatin1String("long")) ||
           (formatType == QLatin1String("medium"))) {
    formatType = QLatin1String("long-timeonly");
    dateFormatType = QLatin1String("long");
    advancedManual = false;
  }

  if (formatType == QLatin1String("long-timeonly"))
    longTimeFormatSelected = true;

  bool timeShowSeconds = _settings->value(QLatin1String("timeShowSeconds"), false).toBool();
  bool timePadHour = _settings->value(QLatin1String("timePadHour"), false).toBool();
  bool timeAMPM = _settings->value(QLatin1String("timeAMPM"), false).toBool();
  mShowTooltip = _settings->value(QLatin1String("showTooltip"), false).toBool();
  // timezone
  bool showTimezone = _settings->value(QLatin1String("showTimezone"), false).toBool() && !longTimeFormatSelected;

  QString timezonePosition = _settings->value(QLatin1String("timezonePosition"), QString()).toString();
  QString timezoneFormatType = _settings->value(QLatin1String("timezoneFormatType"), QString()).toString();

  mTimeZoneWheel = _settings->value(QLatin1String("timeZoneWheel"), true).toBool();

  // date
  bool showDate = _settings->value(QLatin1String("showDate"), false).toBool();

  QString datePosition = _settings->value(QLatin1String("datePosition"), QString()).toString();

  bool dateShowYear = _settings->value(QLatin1String("dateShowYear"), false).toBool();
  bool dateShowDoW = _settings->value(QLatin1String("dateShowDoW"), false).toBool();
  bool datePadDay = _settings->value(QLatin1String("datePadDay"), false).toBool();
  bool dateLongNames = _settings->value(QLatin1String("dateLongNames"), false).toBool();

  // advanced
  QString customFormat = _settings
                             ->value(QLatin1String("customFormat"),
                                     tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'"))
                             .toString();

  if (advancedManual)
    mFormat = customFormat;
  else {
    if (formatType == QLatin1String("short-timeonly"))
      mFormat = QLocale{}.timeFormat(QLocale::ShortFormat);
    else if (formatType == QLatin1String("long-timeonly"))
      mFormat = QLocale{}.timeFormat(QLocale::LongFormat);
    else  // if (formatType == QLatin1String("custom-timeonly"))
      mFormat = QString(QLatin1String("%1:mm%2%3"))
                    .arg(timePadHour ? QLatin1String("hh") : QLatin1String("h"))
                    .arg(timeShowSeconds ? QLatin1String(":ss") : QLatin1String(""))
                    .arg(timeAMPM ? QLatin1String(" A") : QLatin1String(""));

    if (showTimezone) {
      QString timezonePortion;
      if (timezoneFormatType == QLatin1String("short"))
        timezonePortion = QLatin1String("TTTT");
      else if (timezoneFormatType == QLatin1String("long"))
        timezonePortion = QLatin1String("TTTTT");
      else if (timezoneFormatType == QLatin1String("offset"))
        timezonePortion = QLatin1String("T");
      else if (timezoneFormatType == QLatin1String("abbreviation"))
        timezonePortion = QLatin1String("TTT");
      else if (timezoneFormatType == QLatin1String("iana"))
        timezonePortion = QLatin1String("TT");
      else  // if (timezoneFormatType == QLatin1String("custom"))
        timezonePortion = QLatin1String("TTTTTT");

      if (timezonePosition == QLatin1String("below"))
        mFormat = mFormat + QLatin1String("'<br/>'") + timezonePortion;
      else if (timezonePosition == QLatin1String("above"))
        mFormat = timezonePortion + QLatin1String("'<br/>'") + mFormat;
      else if (timezonePosition == QLatin1String("before"))
        mFormat = timezonePortion + QLatin1String(" ") + mFormat;
      else  // if (timezonePosition == QLatin1String("after"))
        mFormat = mFormat + QLatin1String(" ") + timezonePortion;
    }

    if (showDate) {
      QString datePortion;
      if (dateFormatType == QLatin1String("short"))
        datePortion = QLocale{}.dateFormat(QLocale::ShortFormat);
      else if (dateFormatType == QLatin1String("long"))
        datePortion = QLocale{}.dateFormat(QLocale::LongFormat);
      else if (dateFormatType == QLatin1String("iso"))
        datePortion = QLatin1String("yyyy-MM-dd");
      else  // if (dateFormatType == QLatin1String("custom"))
      {
        QString datePortionOrder;
        QString dateLocale = QLocale{}.dateFormat(QLocale::ShortFormat).toLower();
        int yearIndex = dateLocale.indexOf(QLatin1String("y"));
        int monthIndex = dateLocale.indexOf(QLatin1String("m"));
        int dayIndex = dateLocale.indexOf(QLatin1String("d"));
        if (yearIndex < dayIndex)
          // Big-endian (year, month, day) (yyyy MMMM dd, dddd) -> in some Asia countires like China or Japan
          datePortionOrder = QLatin1String("%1%2%3 %4%5%6");
        else if (monthIndex < dayIndex)
          // Middle-endian (month, day, year) (dddd, MMMM dd yyyy) -> USA
          datePortionOrder = QLatin1String("%6%5%3 %4%2%1");
        else
          // Little-endian (day, month, year) (dddd, dd MMMM yyyy) -> most of Europe
          datePortionOrder = QLatin1String("%6%5%4 %3%2%1");
        datePortion =
            datePortionOrder.arg(dateShowYear ? QLatin1String("yyyy") : QLatin1String(""))
                .arg(dateShowYear ? QLatin1String(" ") : QLatin1String(""))
                .arg(dateLongNames ? QLatin1String("MMMM") : QLatin1String("MMM"))
                .arg(datePadDay ? QLatin1String("dd") : QLatin1String("d"))
                .arg(dateShowDoW ? QLatin1String(", ") : QLatin1String(""))
                .arg(dateShowDoW ? (dateLongNames ? QLatin1String("dddd") : QLatin1String("ddd")) : QLatin1String(""));
      }

      if (datePosition == QLatin1String("below"))
        mFormat = mFormat + QLatin1String("'<br/>'") + datePortion;
      else if (datePosition == QLatin1String("above"))
        mFormat = datePortion + QLatin1String("'<br/>'") + mFormat;
      else if (datePosition == QLatin1String("before"))
        mFormat = datePortion + QLatin1String(" ") + mFormat;
      else  // if (datePosition == QLatin1String("after"))
        mFormat = mFormat + QLatin1String(" ") + datePortion;
    }
  }

  if ((oldFormat != mFormat)) {
    int update_interval;
    QString format = mFormat;
    format.replace(regexp, QString());
    // don't support updating on millisecond basis -> big performance hit
    if (format.contains(QLatin1String("s")))
      update_interval = 1000;
    else if (format.contains(QLatin1String("m")))
      update_interval = 60000;
    else
      update_interval = 3600000;

    if (update_interval != mUpdateInterval) {
      mUpdateInterval = update_interval;
      restartTimer();
    }
  }

  bool autoRotate = settings()->value(QLatin1String("autoRotate"), true).toBool();
  if (autoRotate != mAutoRotate) {
    mAutoRotate = autoRotate;
    realign();
  }

  bool showWeekNumber = settings()->value(QLatin1String("showWeekNumber"), true).toBool();
  if (showWeekNumber != mShowWeekNumber) {
    mShowWeekNumber = showWeekNumber;
  }

  if (mPopup) {
    updatePopupContent();
    mPopup->adjustSize();
    mPopup->setGeometry(calculatePopupWindowPos(mPopup->size()));
  }

  setTimeText();
}

QDialog* OneG4WorldClock::configureDialog() {
  return new OneG4WorldClockConfiguration(settings());
}

void OneG4WorldClock::wheelScrolled(int delta) {
  if (mTimeZoneWheel && mTimeZones.count() > 1) {
    mActiveTimeZone = mTimeZones[(mTimeZones.indexOf(mActiveTimeZone) + ((delta > 0) ? -1 : 1) + mTimeZones.size()) %
                                 mTimeZones.size()];
    setTimeText();
  }
}

void OneG4WorldClock::activated(ActivationReason reason) {
  switch (reason) {
    case IOneG4PanelPlugin::Trigger:
    case IOneG4PanelPlugin::MiddleClick:
      break;

    default:
      return;
  }

  if (!mPopup) {
    mPopup = new OneG4WorldClockPopup(mContent);
    connect(mPopup, &QObject::destroyed, this, [this]() {
      mPopupContent = nullptr;
      mPopup = nullptr;
    });

    if (reason == IOneG4PanelPlugin::Trigger) {
      mPopup->setObjectName(QLatin1String("WorldClockCalendar"));

      mPopup->layout()->setContentsMargins(0, 0, 0, 0);
      QCalendarWidget* calendarWidget = new QCalendarWidget(mPopup);
      if (!mShowWeekNumber)
        calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
      mPopup->layout()->addWidget(calendarWidget);

      QString timeZoneName = mActiveTimeZone;
      if (timeZoneName == QLatin1String("local"))
        timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());

      QTimeZone timeZone(timeZoneName.toLatin1());
      calendarWidget->setFirstDayOfWeek(QLocale{}.firstDayOfWeek());
      calendarWidget->setSelectedDate(QDateTime::currentDateTime().toTimeZone(timeZone).date());
    }
    else {
      mPopup->setObjectName(QLatin1String("WorldClockPopup"));

      mPopupContent = new QLabel(mPopup);
      mPopup->layout()->addWidget(mPopupContent);
      mPopupContent->setAlignment(mContent->alignment());

      updatePopupContent();
    }

    mPopup->adjustSize();
    mPopup->setGeometry(calculatePopupWindowPos(mPopup->size()));

    willShowWindow(mPopup);
    mPopup->show();
  }
  else {
    delete mPopup;
  }
}

QString OneG4WorldClock::formatDateTime(const QDateTime& datetime, const QString& timeZoneName) {
  QTimeZone timeZone(timeZoneName.toLatin1());
  QDateTime tzNow = datetime.toTimeZone(timeZone);
  return QLocale::system().toString(tzNow, preformat(mFormat, timeZone, tzNow));
}

void OneG4WorldClock::updatePopupContent() {
  if (mPopupContent) {
    QDateTime now = QDateTime::currentDateTime();
    QStringList allTimeZones;
    bool hasTimeZone = formatHasTimeZone(mFormat);

    for (QString timeZoneName : std::as_const(mTimeZones)) {
      if (timeZoneName == QLatin1String("local"))
        timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());

      QString formatted = formatDateTime(now, timeZoneName);

      if (!hasTimeZone)
        formatted += QLatin1String("<br/>") + QString::fromLatin1(QTimeZone(timeZoneName.toLatin1()).id());

      allTimeZones.append(formatted);
    }

    mPopupContent->setText(allTimeZones.join(QLatin1String("<hr/>")));
  }
}

bool OneG4WorldClock::formatHasTimeZone(QString format) {
  static const QRegularExpression regexp(QLatin1String("'[^']*'"));
  format.replace(regexp, QString());
  return format.contains(QLatin1Char('t'), Qt::CaseInsensitive);
}

QString OneG4WorldClock::preformat(const QString& format, const QTimeZone& timeZone, const QDateTime& dateTime) {
  QString result = format;
  int from = 0;
  for (;;) {
    int apos = result.indexOf(QLatin1Char('\''), from);
    int tz = result.indexOf(QLatin1Char('T'), from);
    if ((apos != -1) && (tz != -1)) {
      if (apos > tz)
        apos = -1;
      else
        tz = -1;
    }
    if (apos != -1) {
      from = apos + 1;
      apos = result.indexOf(QLatin1Char('\''), from);
      if (apos == -1)  // misformat
        break;
      from = apos + 1;
    }
    else if (tz != -1) {
      int length = 1;
      for (; result[tz + length] == QLatin1Char('T'); ++length)
        ;
      if (length > 6)
        length = 6;
      QString replacement;
      switch (length) {
        case 1:
          replacement = timeZone.displayName(dateTime, QTimeZone::OffsetName);
          if (replacement.startsWith(QLatin1String("UTC")))
            replacement = replacement.mid(3);
          break;

        case 2:
          replacement = QString::fromLatin1(timeZone.id());
          break;

        case 3:
          replacement = timeZone.abbreviation(dateTime);
          break;

        case 4:
          replacement = timeZone.displayName(dateTime, QTimeZone::ShortName);
          break;

        case 5:
          replacement = timeZone.displayName(dateTime, QTimeZone::LongName);
          break;

        case 6:
          replacement = mTimeZoneCustomNames[QString::fromLatin1(timeZone.id())];
      }

      if ((tz > 0) && (result[tz - 1] == QLatin1Char('\''))) {
        --tz;
        ++length;
      }
      else
        replacement.prepend(QLatin1Char('\''));

      if (result[tz + length] == QLatin1Char('\''))
        ++length;
      else
        replacement.append(QLatin1Char('\''));

      result.replace(tz, length, replacement);
      from = tz + replacement.length();
    }
    else
      break;
  }
  return result;
}

void OneG4WorldClock::realign() {
  if (mAutoRotate)
    switch (panel()->position()) {
      case IOneG4Panel::PositionTop:
      case IOneG4Panel::PositionBottom:
        mRotatedWidget->setOrigin(Qt::TopLeftCorner);
        break;

      case IOneG4Panel::PositionLeft:
        mRotatedWidget->setOrigin(Qt::BottomLeftCorner);
        break;

      case IOneG4Panel::PositionRight:
        mRotatedWidget->setOrigin(Qt::TopRightCorner);
        break;
    }
  else
    mRotatedWidget->setOrigin(Qt::TopLeftCorner);
  if (mContent->size() != mContent->sizeHint())
    mRotatedWidget->adjustContentSize();
}

ActiveLabel::ActiveLabel(QWidget* parent) : QLabel(parent) {}

void ActiveLabel::wheelEvent(QWheelEvent* event) {
  QPoint angleDelta = event->angleDelta();
  Qt::Orientation orient = (qAbs(angleDelta.x()) > qAbs(angleDelta.y()) ? Qt::Horizontal : Qt::Vertical);
  int delta = (orient == Qt::Horizontal ? angleDelta.x() : angleDelta.y());

  emit wheelScrolled(delta);

  QLabel::wheelEvent(event);
}

void ActiveLabel::mouseReleaseEvent(QMouseEvent* event) {
  switch (event->button()) {
    case Qt::LeftButton:
      emit leftMouseButtonClicked();
      break;

    case Qt::MiddleButton:
      emit middleMouseButtonClicked();
      break;

    default:;
  }

  QLabel::mouseReleaseEvent(event);
}

OneG4WorldClockPopup::OneG4WorldClockPopup(QWidget* parent)
    : QDialog(parent,
              Qt::Window | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup |
                  Qt::X11BypassWindowManagerHint) {
  setAttribute(Qt::WA_DeleteOnClose);
  setLayout(new QHBoxLayout(this));
  layout()->setContentsMargins(1, 1, 1, 1);
}

void OneG4WorldClockPopup::show() {
  QDialog::show();
  activateWindow();
}

bool OneG4WorldClock::eventFilter(QObject* watched, QEvent* event) {
  if (mShowTooltip && watched == mMainWidget && event->type() == QEvent::ToolTip) {
    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
    QDateTime now = QDateTime::currentDateTime();
    QString timeZoneName = mActiveTimeZone;
    if (timeZoneName == QLatin1String("local"))
      timeZoneName = QString::fromLatin1(QTimeZone::systemTimeZoneId());
    QTimeZone timeZone(timeZoneName.toLatin1());
    QDateTime tzNow = now.toTimeZone(timeZone);
    QToolTip::showText(helpEvent->globalPos(), QLocale::system().toString(tzNow, QLocale::ShortFormat));
    return false;
  }
  return QObject::eventFilter(watched, event);
}
