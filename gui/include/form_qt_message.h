#ifndef FORM_QT_MESSAGE_H
#define FORM_QT_MESSAGE_H

#include <QWidget>

namespace Ui {
class FormQtMessage;
}

class FormQtMessageData;

class FormQtMessage : public QWidget
{
    Q_OBJECT

public:
    explicit FormQtMessage(QWidget *parent = 0);
    ~FormQtMessage();

private:
    void initUi();

private:
    Ui::FormQtMessage *ui;
    FormQtMessageData *data_;

public slots:
    void SlotMessageForwarded(const QString &text, int type);

private slots:
    void slotExport();
};

#endif  // FORM_QT_MESSAGE_H
