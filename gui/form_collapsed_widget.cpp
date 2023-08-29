#include "form_collapsed_widget.h"
#include <QApplication>
#include <QCheckBox>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QStyle>
#include "form_fading_button.h"

class FormCollapsedWidgetData
{
public:
    FormCollapsedWidgetData(FormCollapsedWidget *parent);

    void updateControls();

    QWidget *wgt_parent;
    FormFadingButton *fading_btn;
    QHBoxLayout *hlayout;
    QGridLayout *glayout;
    QLabel *lbl_title_icon;
    QLabel *lbl_title;
    QCheckBox *chkbx_title;
    QWidget *wgt_content;
    QPixmap pix_collapsed;
    QPixmap pix_expanded;
    FormCollapsedWidget::WidgetState state;
    bool is_chkbx_valid;
    int margin;
};

FormCollapsedWidgetData::FormCollapsedWidgetData(FormCollapsedWidget *parent)
  : wgt_parent(parent)
  , fading_btn(new FormFadingButton(parent))
  , hlayout(new QHBoxLayout())
  , glayout(new QGridLayout(parent))
  , lbl_title_icon(new QLabel(parent))
  , lbl_title(new QLabel(parent))
  , chkbx_title(new QCheckBox(parent))
  , wgt_content(NULL)
  , state(FormCollapsedWidget::E_COLLAPSED)
  , is_chkbx_valid(false)
  , margin(9)
{
    hlayout->setContentsMargins(margin, margin, margin, margin);
    hlayout->setSpacing(0);

    lbl_title_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    lbl_title_icon->setContentsMargins(0, 0, 0, 0);
    lbl_title_icon->setFixedWidth(0);
    hlayout->addWidget(lbl_title_icon);

    lbl_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    lbl_title->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(lbl_title);

    chkbx_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    chkbx_title->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    chkbx_title->setVisible(false);
    chkbx_title->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(chkbx_title);

    glayout->setContentsMargins(0, 0, 0, 0);
    glayout->setSpacing(0);
    glayout->addLayout(hlayout, 0, 0);
    glayout->addWidget(fading_btn, 0, 2);
}

void FormCollapsedWidgetData::updateControls()
{
    if (wgt_content) {
        wgt_content->setVisible(state == FormCollapsedWidget::E_EXPANDED ||
                                state == FormCollapsedWidget::E_TITLE_NONE ||
                                state == FormCollapsedWidget::E_BUTTON_NONE);
    }

    fading_btn->setChecked(state == FormCollapsedWidget::E_EXPANDED &&
                           wgt_content);

    fading_btn->setVisible(state == FormCollapsedWidget::E_EXPANDED ||
                           state == FormCollapsedWidget::E_COLLAPSED);

    lbl_title_icon->setVisible(state != FormCollapsedWidget::E_TITLE_NONE &&
                               !is_chkbx_valid);

    lbl_title->setVisible(state != FormCollapsedWidget::E_TITLE_NONE &&
                          !is_chkbx_valid);

    chkbx_title->setVisible(state != FormCollapsedWidget::E_TITLE_NONE &&
                            is_chkbx_valid);

    for (QWidget *wgt = wgt_parent; wgt; wgt = wgt->parentWidget()) {
        if (wgt->layout()) {
            wgt->layout()->activate();
        }

        if (QScrollArea *area = qobject_cast<QScrollArea *>(wgt)) {
            QEvent event(QEvent::LayoutRequest);
            QCoreApplication::sendEvent(area, &event);
        }
    }
}

FormCollapsedWidget::FormCollapsedWidget(QWidget *parent)
  : QWidget(parent)
  , data_(new FormCollapsedWidgetData(this))
{
    setLayout(data_->glayout);

    SetCheckBoxValid(false);

    connect(data_->fading_btn,
            &QAbstractButton::toggled,
            this,
            &FormCollapsedWidget::SetExpanded);

    connect(data_->chkbx_title,
            &QAbstractButton::toggled,
            this,
            &FormCollapsedWidget::SigChecked);

    connect(data_->lbl_title,
            &QLabel::linkActivated,
            this,
            &FormCollapsedWidget::SigLinkActivated);

    data_->updateControls();
}

FormCollapsedWidget::~FormCollapsedWidget()
{
    if (data_) {
        delete data_;
        data_ = NULL;
    }
}

FormCollapsedWidget::WidgetState FormCollapsedWidget::State() const
{
    return data_->state;
}

QString FormCollapsedWidget::Title() const
{
    QString title;
    if (data_->is_chkbx_valid) {
        title = data_->chkbx_title->text();
    }

    return title;
}

QWidget *FormCollapsedWidget::Widget() const
{
    return data_->wgt_content;
}

QString FormCollapsedWidget::ButtonText() const
{
    QString text = data_->fading_btn->text();
    return text;
}

QFont FormCollapsedWidget::TitleFont() const
{
    QFont font;
    if (data_->is_chkbx_valid) {
        font = data_->chkbx_title->font();
    }

    return font;
}

bool FormCollapsedWidget::IsChecked() const
{
    bool checked = false;
    if (data_->is_chkbx_valid) {
        checked = data_->chkbx_title->isChecked();
    }

    return checked;
}

bool FormCollapsedWidget::IsCheckBoxValid() const
{
    bool valid = data_->is_chkbx_valid;
    return valid;
}

void FormCollapsedWidget::SetState(FormCollapsedWidget::WidgetState state)
{
    if (state == data_->state) {
        return;
    }

    data_->state = state;
    data_->updateControls();

    emit SigExpanded(data_->state == E_EXPANDED);
}

void FormCollapsedWidget::SetTitle(const QString &text)
{
    data_->chkbx_title->setText(text);
    data_->lbl_title->setText(text);
}

void FormCollapsedWidget::SetWidget(QWidget *widget)
{
    if (data_->wgt_content == widget) {
        return;
    }

    if (data_->wgt_content) {
        data_->glayout->removeWidget(data_->wgt_content);
        delete data_->wgt_content;
        data_->wgt_content = NULL;
    }

    data_->wgt_content = widget;
    if (data_->wgt_content) {
        data_->wgt_content->setContentsMargins(data_->margin,
                                               data_->margin,
                                               data_->margin,
                                               data_->margin);

        data_->glayout->addWidget(data_->wgt_content, 2, 0, 1, 3);
    }

    data_->updateControls();
}

void FormCollapsedWidget::SetButtonText(const QString &text)
{
    data_->fading_btn->setText(text);
}

void FormCollapsedWidget::SetTitleFont(const QFont &font)
{
    if (data_->is_chkbx_valid) {
        data_->chkbx_title->setFont(font);
    }

    data_->lbl_title->setFont(font);
}

void FormCollapsedWidget::SetChecked(bool checked)
{
    data_->chkbx_title->setChecked(checked);
}

void FormCollapsedWidget::SetCheckBoxValid(bool is_valid)
{
    data_->is_chkbx_valid = is_valid;
    data_->updateControls();
}

void FormCollapsedWidget::SetIcon(const QIcon &icon)
{
    int ico_size = style()->pixelMetric(QStyle::PM_ButtonIconSize, 0, this);
    data_->lbl_title_icon->setFixedWidth(icon.isNull() ? 0 : ico_size);
    data_->lbl_title_icon->setPixmap(icon.pixmap(ico_size, ico_size));
    data_->chkbx_title->setIcon(icon);

    if (data_->is_chkbx_valid) {
        data_->hlayout->setSpacing(0);
    } else {
        data_->hlayout->setSpacing(icon.isNull() ? 0 : 6);
    }
}

void FormCollapsedWidget::SetExpanded(bool expanded)
{
    SetState(expanded ? E_EXPANDED : E_COLLAPSED);
}

QWidget *FormCollapsedWidget::TakeWidget()
{
    QWidget *widget = data_->wgt_content;
    data_->wgt_content = 0;
    data_->glayout->removeWidget(widget);
    if (widget) {
        widget->setParent(0);
    }

    return widget;
}

QPixmap FormCollapsedWidget::CreateBackground(const QSize &size,
                                              QWidget *widget)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    QRect full_rect(0, 0, size.width(), size.height());
    p.fillRect(full_rect, widget->palette().color(QPalette::Window));
    p.setRenderHint(QPainter::Antialiasing, true);
    p.translate(0.5, 0.5);
    p.setPen(QColor(0, 0, 0, 40));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(full_rect.adjusted(0, 0, -1, -1), 2, 2);
    p.setBrush(Qt::NoBrush);
    p.setPen(QColor(255, 255, 255, 140));
    p.drawRoundedRect(full_rect.adjusted(1, 1, -2, -2), 2, 2);
    p.setPen(QPen(widget->palette().color(QPalette::Mid)));

    return pixmap;
}

void FormCollapsedWidget::paintEvent(QPaintEvent *paint_event)
{
    QWidget::paintEvent(paint_event);

    QPainter p(this);
    QWidget *top_left_wgt = data_->is_chkbx_valid
                                ? static_cast<QWidget *>(data_->chkbx_title)
                                : static_cast<QWidget *>(data_->lbl_title_icon);

    QPoint top_left(top_left_wgt->geometry().left() - data_->margin,
                    contentsRect().top());

    const QRect paint_area(top_left, contentsRect().bottomRight());
    if (data_->state == FormCollapsedWidget::E_COLLAPSED) {
        if (data_->pix_collapsed.isNull() ||
            data_->pix_collapsed.size() != size()) {
            data_->pix_collapsed = CreateBackground(paint_area.size(), this);
        }

        p.drawPixmap(paint_area, data_->pix_collapsed);

    } else {
        if (data_->pix_expanded.isNull() ||
            data_->pix_expanded.size() != size()) {
            data_->pix_expanded = CreateBackground(paint_area.size(), this);
        }

        p.drawPixmap(paint_area, data_->pix_expanded);
    }
}
