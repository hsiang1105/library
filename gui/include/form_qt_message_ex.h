#ifndef FORM_QT_MESSAGE_EX_H
#define FORM_QT_MESSAGE_EX_H

#include <QWidget>

class QStandardItemModel;
class QSortFilterProxyModel;

namespace Ui {
class FormQtMessageEx;
}

class FormQtMessageEx : public QWidget
{
    Q_OBJECT

public:
    explicit FormQtMessageEx(QWidget *parent = 0);
    ~FormQtMessageEx();

private:
    void initUi();
    void initModelView();

private:
    Ui::FormQtMessageEx *ui;
    QStandardItemModel *model_;
    QSortFilterProxyModel *filter_model_;

public slots:
    void SlotMessageForwarded(const QString &text, int type);

private slots:
    void slotCheckBoxClicked(bool clicked);
    void slotExport();
    void slotClear();
};

#endif  // FORM_QT_MESSAGE_EX_H
