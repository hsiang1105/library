#include "form_capacity_bar.h"
#include <qdrawutil.h>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionProgressBar>

class FormCapacityBarData
{
public:
    FormCapacityBarData()
      : warning_percent(80)
      , critical_percent(90)
    {
        color_map.insert(FormCapacityBar::E_NORMAL, QColor(0, 176, 180));
        color_map.insert(FormCapacityBar::E_WARNING, QColor(255, 192, 0));
        color_map.insert(FormCapacityBar::E_CRITICAL, QColor(214, 77, 84));
    }

    QMap<int, QColor> color_map;
    int warning_percent;
    int critical_percent;
};

FormCapacityBar::FormCapacityBar(QWidget *parent)
  : QProgressBar(parent)
  , data_(new FormCapacityBarData)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

FormCapacityBar::~FormCapacityBar()
{
    if (data_) {
        delete data_;
        data_ = NULL;
    }
}

QColor FormCapacityBar::BarColor(Level level) const
{
    QColor color = data_->color_map.value(level);

    return color;
}

int FormCapacityBar::WarningPercent() const
{
    return data_->warning_percent;
}

int FormCapacityBar::CriticalPercent() const
{
    return data_->critical_percent;
}

void FormCapacityBar::SetBarColor(const QColor &color, Level level)
{
    data_->color_map.insert(level, color);
    repaint();
}

void FormCapacityBar::SetWarningPercent(int percent)
{
    if (percent > 100) {
        percent = 100;
    } else if (percent < 0) {
        percent = 0;
    }

    data_->warning_percent = percent;
    repaint();
}

void FormCapacityBar::SetCriticalPercent(int percent)
{
    if (percent > 100) {
        percent = 100;
    } else if (percent < 0) {
        percent = 0;
    }

    data_->critical_percent = percent;
    repaint();
}

void FormCapacityBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QStyleOptionProgressBar opt;
    initStyleOption(&opt);

    QRect rect;
    rect = style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, this);
    qDrawShadePanel(&painter,
                    rect,
                    opt.palette,
                    true,
                    1,
                    &opt.palette.brush(QPalette::Window));

    rect = style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, this);
    int pos = QStyle::sliderPositionFromValue(minimum(),
                                              maximum(),
                                              value(),
                                              rect.width());

    QBrush brush(BarColor(E_NORMAL));
    int percent = round((double)pos / rect.width() * 100);
    if (percent >= data_->critical_percent) {
        brush.setColor(BarColor(E_CRITICAL));
    } else if (percent >= data_->warning_percent &&
               percent < data_->critical_percent) {
        brush.setColor(BarColor(E_WARNING));
    }

    rect.setWidth(pos);
    qDrawPlainRect(&painter, rect, Qt::transparent, 1, &brush);

    QStyleOptionProgressBar opt_label(opt);
    rect = style()->subElementRect(QStyle::SE_ProgressBarLabel, &opt, this);
    opt_label.rect = rect;
    style()->drawControl(QStyle::CE_ProgressBarLabel,
                         &opt_label,
                         &painter,
                         this);
}
