#include "form_loading_spinner.h"
#include <QPainter>
#include <QTimer>

class FormLoadingSpinnerData
{
public:
    FormLoadingSpinnerData()
      : always_displayed(false)
      , angle(0)
      , color(Qt::black)
      , timer(NULL)
    {
    }

    bool always_displayed;
    int angle;
    QColor color;
    QTimer *timer;
};

FormLoadingSpinner::FormLoadingSpinner(QWidget *parent)
  : QWidget(parent)
  , data_(new FormLoadingSpinnerData)
{
    data_->timer = new QTimer(this);
    data_->timer->setInterval(50);
    connect(data_->timer,
            &QTimer::timeout,
            this,
            &FormLoadingSpinner::slotTimeout);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

FormLoadingSpinner::~FormLoadingSpinner()
{
    if (data_) {
        delete data_;
        data_ = NULL;
    }
}

bool FormLoadingSpinner::IsAnimated() const
{
    return data_->timer->isActive();
}

bool FormLoadingSpinner::IsAlwaysDisplayed() const
{
    return data_->always_displayed;
}

int FormLoadingSpinner::AnimationDealy() const
{
    return data_->timer->interval();
}

const QColor &FormLoadingSpinner::Color() const
{
    return data_->color;
}

void FormLoadingSpinner::StartAnimation()
{
    data_->angle = 0;
    data_->timer->start();
}

void FormLoadingSpinner::StopAnimation()
{
    data_->timer->stop();
    update();
}

void FormLoadingSpinner::SetAlwaysDisplay(bool enabled)
{
    data_->always_displayed = enabled;
}

void FormLoadingSpinner::SetAnimationDelay(int delay)
{
    data_->timer->start(delay);
}

void FormLoadingSpinner::SetColor(const QColor &color)
{
    data_->color = color;
    update();
}

void FormLoadingSpinner::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!IsAnimated() && !IsAlwaysDisplayed()) {
        return;
    }

    int w = qMin(width(), height());
    int outer_r = (w - 1) / 2;
    int inner_r = (w - 1) / 5;
    int capsule_h = outer_r - inner_r;
    int capsule_w = capsule_h / 3;
    int capsule_r = capsule_w / 2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < 12; i++) {
        QColor color = data_->color;
        color.setAlphaF(1.0f - (i / 12.0f));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.save();
        painter.translate(rect().center());
        painter.rotate(data_->angle - i * 30.0f);
        painter.drawRoundedRect(-(capsule_w / 2),
                                -(inner_r + capsule_h),
                                capsule_w,
                                capsule_h,
                                capsule_r,
                                capsule_r);

        painter.restore();
    }
}

void FormLoadingSpinner::slotTimeout()
{
    data_->angle = (data_->angle + 30) % 360;
    update();
}
