#include "form_fading_button.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStyleOption>

FormFadingButton::FormFadingButton(QWidget *parent)
  : QAbstractButton(parent)
  , fader_(0.0)
{
    setCheckable(true);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    setText(tr("Details"));
}

float FormFadingButton::Fader() const
{
    return fader_;
}

void FormFadingButton::SetFader(float value)
{
    fader_ = value;
    update();
}

QSize FormFadingButton::sizeHint() const
{
    // TODO: Adjust this when icons become available!
    const int w = fontMetrics().width(text()) + 32;
    return QSize(w, 22);
}

void FormFadingButton::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter p(this);

    // draw hover animation
    if (!isDown() && fader_ > 0) {
        QColor c = palette().light().color();
        c.setAlpha(int(fader_ * c.alpha()));

        QRect r = rect();
        r.adjust(1, 1, -2, -2);
        p.fillRect(r, c);
    }

    if (isChecked()) {
        if (pix_checked_.isNull() ||
            pix_checked_.size() / pix_checked_.devicePixelRatio() !=
                contentsRect().size()) {
            pix_checked_ = contentPixmap(contentsRect().size(), true);
        }

        p.drawPixmap(contentsRect(), pix_checked_);

    } else {
        if (pix_unchecked_.isNull() ||
            pix_unchecked_.size() / pix_unchecked_.devicePixelRatio() !=
                contentsRect().size()) {
            pix_unchecked_ = contentPixmap(contentsRect().size(), false);
        }

        p.drawPixmap(contentsRect(), pix_unchecked_);
    }

    if (isDown()) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 20));
        p.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 1, 1);
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &option, &p, this);
    }
}

bool FormFadingButton::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::Enter: {
            QPropertyAnimation *animation =
                new QPropertyAnimation(this, "fader");

            animation->setDuration(200);
            animation->setEndValue(1.0);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        } break;

        case QEvent::Leave: {
            QPropertyAnimation *animation =
                new QPropertyAnimation(this, "fader");

            animation->setDuration(200);
            animation->setEndValue(0.0);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        } break;

        default:
            return QAbstractButton::event(event);
    }

    return false;
}

QPixmap FormFadingButton::contentPixmap(const QSize &size, bool checked)
{
    const qreal pixel_ratio = devicePixelRatio();
    QPixmap pixmap(size * pixel_ratio);
    pixmap.setDevicePixelRatio(pixel_ratio);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(0.5, 0.5);
    p.setPen(QColor(255, 255, 255, 140));
    p.drawRoundedRect(1, 1, size.width() - 3, size.height() - 3, 1, 1);
    p.setPen(QPen(QColor(0, 0, 0, 40)));
    p.drawLine(0, 1, 0, size.height() - 2);
    if (checked) {
        p.drawLine(1, size.height() - 1, size.width() - 1, size.height() - 1);
    }

    p.setPen(palette().color(QPalette::Text));

    QRect text_rect = p.fontMetrics().boundingRect(text());
    text_rect.setWidth(text_rect.width() + 15);
    text_rect.setHeight(text_rect.height() + 4);
    text_rect.moveCenter(rect().center());

    p.drawText(text_rect, Qt::AlignLeft | Qt::AlignVCenter, text());

    int arrow_size = 15;
    QStyleOption arrow_opt;
    arrow_opt.initFrom(this);

    QPalette palette = arrow_opt.palette;
    palette.setBrush(QPalette::All, QPalette::Text, QColor(0, 0, 0));
    arrow_opt.rect = QRect(size.width() - arrow_size - 6,
                           height() / 2 - arrow_size / 2,
                           arrow_size,
                           arrow_size);

    arrow_opt.palette = palette;

    if (checked) {
        style()->drawPrimitive(QStyle::PE_IndicatorArrowUp,
                               &arrow_opt,
                               &p,
                               this);

    } else {
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
                               &arrow_opt,
                               &p,
                               this);
    }

    return pixmap;
}
