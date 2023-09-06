#include "form_qt_message.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QTextStream>
#include "ui_form_qt_message.h"

class FormQtMessageData
{
public:
    FormQtMessageData()
      : visible_count(100)
    {
    }

    int visible_count;
    QStringList debug_messages;
    QStringList info_messages;
};

FormQtMessage::FormQtMessage(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::FormQtMessage)
  , data_(new FormQtMessageData)
{
    ui->setupUi(this);
    initUi();
}

FormQtMessage::~FormQtMessage()
{
    if (data_) {
        delete data_;
        data_ = NULL;
    }

    delete ui;
}

void FormQtMessage::initUi()
{
    ui->chkbx_dbg->setChecked(true);
    ui->chkbx_info->setChecked(false);
    ui->tabwgt_dbg->show();
    ui->tabwgt_info->hide();

    connect(ui->chkbx_dbg,
            &QCheckBox::clicked,
            ui->tabwgt_dbg,
            &QTabWidget::setVisible);

    connect(ui->chkbx_info,
            &QCheckBox::clicked,
            ui->tabwgt_info,
            &QTabWidget::setVisible);

    connect(ui->tbtn_clear_dbg, &QToolButton::clicked, this, [=]() {
        ui->listwgt_dbg->clear();
        data_->debug_messages.clear();
    });

    connect(ui->tbtn_clear_info, &QToolButton::clicked, this, [=]() {
        ui->listwgt_info->clear();
        data_->info_messages.clear();
    });

    connect(ui->tbtn_export_dbg,
            &QToolButton::clicked,
            this,
            &FormQtMessage::slotExport);

    connect(ui->tbtn_export_info,
            &QToolButton::clicked,
            this,
            &FormQtMessage::slotExport);
}

void FormQtMessage::SlotMessageForwarded(const QString &text, int type)
{
    if (type == QtInfoMsg) {
        if (ui->listwgt_info->count() > data_->visible_count) {
            QListWidgetItem *item = ui->listwgt_info->takeItem(0);
            delete item;
        }

        QListWidgetItem *item = new QListWidgetItem(text);
        item->setForeground(QBrush(Qt::darkGreen));
        ui->listwgt_info->addItem(item);
        ui->listwgt_info->scrollToBottom();

        data_->info_messages << text;

    } else {
        if (ui->listwgt_dbg->count() > data_->visible_count) {
            QListWidgetItem *item = ui->listwgt_dbg->takeItem(0);
            delete item;
        }

        QListWidgetItem *item = new QListWidgetItem(text);

        if (type == QtDebugMsg) {
            item->setForeground(QBrush(Qt::darkBlue));

        } else if (type == QtWarningMsg) {
            item->setForeground(QBrush(Qt::darkYellow));

        } else if (type == QtCriticalMsg) {
            item->setForeground(QBrush(Qt::darkMagenta));

        } else if (type == QtFatalMsg) {
            item->setForeground(QBrush(Qt::darkRed));
        }

        ui->listwgt_dbg->addItem(item);
        ui->listwgt_dbg->scrollToBottom();

        data_->debug_messages << text;
    }
}

void FormQtMessage::slotExport()
{
    QStringList messages;
    if (ui->tbtn_export_dbg == sender()) {
        messages = data_->debug_messages;
    } else if (ui->tbtn_export_info == sender()) {
        messages = data_->info_messages;
    } else {
        return;
    }

    QString dir =
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Export"),
                                                dir,
                                                tr("Text file(*.txt)"));

    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (const QString &message : messages) {
            out << message << endl;
        }

        file.close();
    }
}
