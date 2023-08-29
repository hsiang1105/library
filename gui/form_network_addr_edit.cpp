#include "form_network_addr_edit.h"
#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QRegExpValidator>
#include <QStyleOption>

class FormNetworkAddrEditData
{
public:
    FormNetworkAddrEditData()
      : type(FormNetworkAddrEdit::E_IPV4)
      , frame_enabled(true)
      , has_focus(false)
      , has_hover(false)
    {
    }

    FormNetworkAddrEdit::AddressType type;
    int lbl_count;
    int max_digits;
    bool frame_enabled;
    bool has_focus;
    bool has_hover;
    QString sep_text;
    QString rx_pattern;
    QList<QLineEdit *> ledts;
};

FormNetworkAddrEdit::FormNetworkAddrEdit(QWidget *parent)
  : QWidget(parent)
  , data_(new FormNetworkAddrEditData)
{
    initUi();
}

FormNetworkAddrEdit::~FormNetworkAddrEdit()
{
    if (data_) {
        delete data_;
        data_ = NULL;
    }
}

FormNetworkAddrEdit::AddressType FormNetworkAddrEdit::Type() const
{
    return data_->type;
}

QString FormNetworkAddrEdit::Text()
{
    QStringList text_list;
    for (QLineEdit *ledt : data_->ledts) {
        if (ledt) {
            QString text = ledt->text();
            fixUpText(text);
            text_list << text;
        }
    }

    QString text = text_list.join(data_->sep_text);

    return text;
}

void FormNetworkAddrEdit::SetType(AddressType type)
{
    data_->type = type;
    clearUi();
    initUi();
}

void FormNetworkAddrEdit::SetText(const QString &text)
{
    if (!IsValid(text, data_->type)) {
        qDebug() << "[FormNetworkAddrEdit] validte text failed!";
        return;
    }

    QStringList text_list = text.split(data_->sep_text);
    if (text_list.count() != data_->lbl_count) {
        qDebug() << "[FormNetworkAddrEdit] text format error!";
        return;
    }

    int index = 0;
    for (const QString &text : text_list) {
        QString fixed_text(text);
        fixUpText(fixed_text);
        data_->ledts.at(index)->setText(fixed_text);
        data_->ledts.at(index)->clearFocus();
        index++;
    }
}

void FormNetworkAddrEdit::SetFrame(bool enabled)
{
    data_->frame_enabled = enabled;
    update();
}

bool FormNetworkAddrEdit::IsValid(const QString &text, AddressType type)
{
    QString pattern;
    switch (type) {
        case E_IPV4:
        case E_CUSTOM:
            pattern = QString(
                "((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|["
                "01]?\\d\\d?)");
            break;

        case E_IPV6:
            pattern = QString("([\\da-fA-F]{1,4}:){7}[\\da-fA-F]{1,4}");
            break;

        case E_MAC:
            pattern = QString("([\\da-fA-F]{1,2}-){5}[\\da-fA-F]{1,2}");
            break;

        default:
            return false;
            break;
    }

    QRegExp rx(pattern);
    int pos = 0;
    pos = rx.indexIn(text, pos);
    if (pos != -1) {
        if (rx.matchedLength() == text.size()) {
            return true;
        }
    }

    return false;
}

bool FormNetworkAddrEdit::eventFilter(QObject *obj, QEvent *event)
{
    bool res = QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *kevent = static_cast<QKeyEvent *>(event);
        if (kevent) {
            for (int i = 0; i != data_->lbl_count; ++i) {
                QLineEdit *ledt = data_->ledts.at(i);
                if (ledt == obj) {
                    switch (kevent->key()) {
                        case Qt::Key_Left:
                            if (ledt->cursorPosition() == 0) {
                                // user wants to move to previous item
                                movePrevLineEdit(i);
                            }
                            break;

                        case Qt::Key_Right:
                            if (ledt->text().isEmpty() ||
                                (ledt->text().size() ==
                                 ledt->cursorPosition())) {
                                // user wants to move to next item
                                moveNextLineEdit(i);
                            }
                            break;

                        case Qt::Key_Backspace:
                            if (ledt->text().isEmpty() ||
                                ledt->cursorPosition() == 0) {
                                // user wants to move to previous item
                                movePrevLineEdit(i);
                            }
                            break;

                        case Qt::Key_Minus:
                        case Qt::Key_Period:
                        case Qt::Key_Colon:
                            moveNextLineEdit(i);
                            break;

                        case Qt::Key_0:
                            if (data_->type == E_IPV4 ||
                                data_->type == E_CUSTOM) {
                                if (ledt->text().isEmpty()) {
                                    moveNextLineEdit(i);
                                }
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
        }
    } else if (event->type() == QEvent::HoverEnter) {
        data_->has_hover = true;
        update();

    } else if (event->type() == QEvent::HoverLeave) {
        data_->has_hover = false;
        update();

    } else if (event->type() == QEvent::FocusIn) {
        data_->has_focus = true;
        update();

    } else if (event->type() == QEvent::FocusOut) {
        data_->has_focus = false;
        update();
    }

    return res;
}

void FormNetworkAddrEdit::paintEvent(QPaintEvent *event)
{
    ensurePolished();

    QPainter painter(this);
    QStyle *style = this->style();
    QStyleOptionFrame option;
    option.initFrom(this);
    option.rect = this->contentsRect();
    option.lineWidth =
        data_->frame_enabled
            ? style->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this)
            : 0;

    option.midLineWidth = 0;
    option.state |= QStyle::State_Sunken;

    if (data_->has_focus) {
        option.state |= QStyle::State_HasFocus;
    } else {
        option.state &= (~QStyle::State_HasFocus);
    }

    if (data_->has_hover) {
        option.state |= QStyle::State_MouseOver;
    } else {
        option.state &= (~QStyle::State_MouseOver);
    }

    style->drawPrimitive(QStyle::PE_PanelLineEdit, &option, &painter, this);

    QWidget::paintEvent(event);
}

QSize FormNetworkAddrEdit::sizeHint() const
{
    ensurePolished();

    int vertical_margin = 1;
    int horizontal_margin = 2;
    int h = 18 + (vertical_margin << 1);
    int w = 9 * data_->max_digits * data_->lbl_count + (horizontal_margin << 1);

    if (!data_->ledts.isEmpty()) {
        QLineEdit *ledt = data_->ledts.first();
        QFontMetrics fm = ledt->fontMetrics();
        int part_h = qMax(fm.height(), 14) + (vertical_margin << 1);
        int part_w = fm.width(QLatin1Char('0')) * data_->max_digits +
                     (horizontal_margin << 1);

        part_w = part_w * data_->lbl_count;
        h = qMax(h, part_h);
        w = qMax(w, part_w);
    }

    return QSize(w, h).expandedTo(QApplication::globalStrut());
}

QSize FormNetworkAddrEdit::minimumSizeHint() const
{
    ensurePolished();

    int vertical_margin = 1;
    int horizontal_margin = 2;
    int h = 18 + (vertical_margin << 1);
    int w = 9 * data_->lbl_count + (horizontal_margin << 1);

    if (!data_->ledts.isEmpty()) {
        QLineEdit *ledt = data_->ledts.first();
        QFontMetrics fm = ledt->fontMetrics();
        int part_h = fm.height() + qMax((vertical_margin << 1), fm.leading());
        int part_w = fm.width(QLatin1Char('0')) + (horizontal_margin << 1);

        part_w = part_w * data_->lbl_count;
        h = qMax(h, part_h);
        w = qMax(w, part_w);
    }

    return QSize(w, h).expandedTo(QApplication::globalStrut());
}

void FormNetworkAddrEdit::preProcess()
{
    switch (data_->type) {
        case E_IPV4:
            data_->lbl_count = 4;
            data_->max_digits = 3;
            data_->sep_text = QString(".");
            data_->rx_pattern = QString(
                "^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
            break;

        case E_IPV6:
            data_->lbl_count = 8;
            data_->max_digits = 4;
            data_->sep_text = QString(":");
            data_->rx_pattern = QString("^([\\da-fA-F]{1,4})$");
            break;

        case E_MAC:
            data_->lbl_count = 6;
            data_->max_digits = 2;
            data_->sep_text = QString("-");
            data_->rx_pattern = QString("^([\\da-fA-F]{2,2})$");
            break;

        case E_CUSTOM:
            data_->lbl_count = 4;
            data_->max_digits = 3;
            data_->sep_text = QString(".");
            data_->rx_pattern = QString(
                "^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
            break;

        default:
            data_->lbl_count = 0;
            data_->max_digits = 0;
            data_->rx_pattern = QString();
            break;
    }
}

void FormNetworkAddrEdit::postProcess()
{
    switch (data_->type) {
        case E_CUSTOM: {
            QHBoxLayout *hlayout = static_cast<QHBoxLayout *>(this->layout());
            if (hlayout) {
                for (int i = 0; i != 2; ++i) {
                    QLabel *lbl = createLabel();
                    hlayout->addWidget(lbl);
                    hlayout->setStretch(hlayout->count(), 0);

                    QLineEdit *ledt = createLineEdit();
                    ledt->setReadOnly(true);
                    ledt->setText("1");

                    hlayout->addWidget(ledt);
                    hlayout->setStretch(hlayout->count(), 1);
                }
            }
        } break;

        default:
            break;
    }
}

void FormNetworkAddrEdit::clearUi()
{
    data_->ledts.clear();

    QLayout *layout = this->layout();
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != 0) {
            QWidget *wgt = child->widget();
            if (wgt) {
                delete wgt;
            }
            delete child;
        }

        delete layout;
    }
}

void FormNetworkAddrEdit::initUi()
{
    preProcess();

    QHBoxLayout *hlayout = new QHBoxLayout(this);
    this->setLayout(hlayout);
    hlayout->setContentsMargins(2, 0, 2, 0);
    hlayout->setSpacing(0);

    for (int i = 0; i != data_->lbl_count; ++i) {
        if (i != 0) {
            QLabel *lbl = createLabel();
            hlayout->addWidget(lbl);
            hlayout->setStretch(hlayout->count(), 0);
        }

        QLineEdit *ledt = createLineEdit();
        ledt->installEventFilter(this);
        data_->ledts << ledt;

        QRegExp rx(data_->rx_pattern);
        QValidator *validator = new QRegExpValidator(rx, ledt);
        ledt->setValidator(validator);

        hlayout->addWidget(ledt);
        hlayout->setStretch(hlayout->count(), 1);

        connect(ledt,
                &QLineEdit::textChanged,
                this,
                &FormNetworkAddrEdit::slotTextChanged);
    }

    postProcess();
}

void FormNetworkAddrEdit::moveNextLineEdit(int index)
{
    if (index + 1 != data_->lbl_count) {
        QLineEdit *ledt = data_->ledts.at(index + 1);
        ledt->setCursorPosition(0);
        ledt->setFocus();
        ledt->selectAll();
    }
}

void FormNetworkAddrEdit::movePrevLineEdit(int index)
{
    if (index != 0) {
        QLineEdit *ledt = data_->ledts.at(index - 1);
        ledt->setCursorPosition(ledt->text().size());
        ledt->setFocus();
        // ledt->selectAll();
    }
}

void FormNetworkAddrEdit::fixUpText(QString &text)
{
    if (data_->type == E_IPV6 || data_->type == E_MAC) {
        text = QString("%1").arg(text, data_->max_digits, QLatin1Char('0'));
    }
}

QLineEdit *FormNetworkAddrEdit::createLineEdit()
{
    QLineEdit *ledt = new QLineEdit(this);
    ledt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ledt->setMinimumSize(QSize(0, 0));
    ledt->setMaximumSize(QSize(16777215, 16777215));
    ledt->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ledt->setAlignment(Qt::AlignCenter);

    ledt->setStyleSheet(
        "QLineEdit {background-color: transparent; border: none;} "
        "QLineEdit:hover {background-color: transparent; border: none;} "
        "QLineEdit:focus {background-color: transparent; border: none;}");

    return ledt;
}

QLabel *FormNetworkAddrEdit::createLabel()
{
    QLabel *lbl = new QLabel(data_->sep_text);
    lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    lbl->setMinimumSize(QSize(0, 0));
    lbl->setMaximumSize(QSize(16777215, 16777215));
    lbl->setAlignment(Qt::AlignCenter);

    return lbl;
}

void FormNetworkAddrEdit::slotTextChanged(const QString &text)
{
    QLineEdit *cur_ledt = static_cast<QLineEdit *>(sender());
    if (cur_ledt == NULL) {
        return;
    }

    if ((text.size() == data_->max_digits &&
         text.size() == cur_ledt->cursorPosition())) {
        // auto-move to next item
        int index = 0;
        for (QLineEdit *ledt : data_->ledts) {
            if (ledt == cur_ledt) {
                break;
            }
            index++;
        }

        moveNextLineEdit(index);
    }
}
