#include "form_qt_message_ex.h"
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QTextStream>
#include "ui_form_qt_message_ex.h"

FormQtMessageEx::FormQtMessageEx(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::FormQtMessageEx)
  , model_(NULL)
{
    ui->setupUi(this);
    initUi();
}

FormQtMessageEx::~FormQtMessageEx()
{
    delete ui;
}

void FormQtMessageEx::initUi()
{
    initModelView();

    connect(ui->chkbx_debug,
            &QCheckBox::clicked,
            this,
            &FormQtMessageEx::slotCheckBoxClicked);

    connect(ui->chkbx_info,
            &QCheckBox::clicked,
            this,
            &FormQtMessageEx::slotCheckBoxClicked);

    connect(ui->chkbx_warning,
            &QCheckBox::clicked,
            this,
            &FormQtMessageEx::slotCheckBoxClicked);

    connect(ui->chkbx_critical,
            &QCheckBox::clicked,
            this,
            &FormQtMessageEx::slotCheckBoxClicked);

    connect(ui->chkbx_fatal,
            &QCheckBox::clicked,
            this,
            &FormQtMessageEx::slotCheckBoxClicked);

    connect(ui->tbtn_export,
            &QToolButton::clicked,
            this,
            &FormQtMessageEx::slotExport);

    connect(ui->tbtn_clear,
            &QToolButton::clicked,
            this,
            &FormQtMessageEx::slotClear);
}

void FormQtMessageEx::initModelView()
{
    model_ = new QStandardItemModel(this);
    filter_model_ = new QSortFilterProxyModel(this);

    QStringList labels = {tr("Type"), tr("Message")};
    model_->setHorizontalHeaderLabels(labels);

    filter_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filter_model_->setFilterRole(Qt::UserRole + 1);
    filter_model_->setFilterKeyColumn(0);
    filter_model_->setSourceModel(model_);
    ui->tableview->setModel(filter_model_);
    ui->tableview->horizontalHeader()->setStretchLastSection(true);
}

void FormQtMessageEx::SlotMessageForwarded(const QString &text, int type)
{
    if (model_->rowCount() > 100) {
        model_->removeRow(0);
    }

    Qt::GlobalColor color = Qt::black;
    QStandardItem *item_type = new QStandardItem;
    QStandardItem *item_msg = new QStandardItem(text);
    switch (type) {
        case QtInfoMsg:
            color = Qt::darkGreen;
            item_type->setIcon(
                style()->standardIcon(QStyle::SP_MessageBoxInformation));

            item_type->setText(tr("Info"));
            break;

        case QtDebugMsg:
            color = Qt::darkBlue;
            item_type->setIcon(
                style()->standardIcon(QStyle::SP_MessageBoxInformation));

            item_type->setText(tr("Debug"));
            break;

        case QtWarningMsg:
            color = Qt::darkYellow;
            item_type->setIcon(
                style()->standardIcon(QStyle::SP_MessageBoxWarning));

            item_type->setText(tr("Warning"));
            break;

        case QtCriticalMsg:
            color = Qt::darkMagenta;
            item_type->setIcon(
                style()->standardIcon(QStyle::SP_MessageBoxCritical));

            item_type->setText(tr("Critical"));
            break;

        case QtFatalMsg:
            color = Qt::darkRed;
            item_type->setIcon(
                style()->standardIcon(QStyle::SP_MessageBoxCritical));

            item_type->setText(tr("Fatal"));
            break;

        default:
            delete item_type;
            delete item_msg;
            return;
            break;
    }

    item_type->setData(type);
    item_type->setForeground(QBrush(color));
    item_msg->setForeground(QBrush(color));

    QList<QStandardItem *> items = {item_type, item_msg};
    model_->appendRow(items);

    ui->tableview->scrollToBottom();
}

void FormQtMessageEx::slotCheckBoxClicked(bool clicked)
{
    Q_UNUSED(clicked)

    QStringList list;
    if (ui->chkbx_info->isChecked()) {
        list << QString("(\\b%1\\b)").arg((int)QtInfoMsg);
    }

    if (ui->chkbx_debug->isChecked()) {
        list << QString("(\\b%1\\b)").arg((int)QtDebugMsg);
    }

    if (ui->chkbx_warning->isChecked()) {
        list << QString("(\\b%1\\b)").arg((int)QtWarningMsg);
    }

    if (ui->chkbx_critical->isChecked()) {
        list << QString("(\\b%1\\b)").arg((int)QtCriticalMsg);
    }

    if (ui->chkbx_fatal->isChecked()) {
        list << QString("(\\b%1\\b)").arg((int)QtFatalMsg);
    }

    QString pattern;
    if (!list.isEmpty()) {
        pattern = list.join("|");
    }

    QRegExp reg_ex(pattern, Qt::CaseSensitive);
    filter_model_->setFilterRegExp(reg_ex);
}

void FormQtMessageEx::slotExport()
{
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

        for (int row = 0; row < model_->rowCount(); ++row) {
            out << model_->item(row, 1)->text() << endl;
        }

        file.close();
    }
}

void FormQtMessageEx::slotClear()
{
    int count = model_->rowCount();
    model_->removeRows(0, count);
}
