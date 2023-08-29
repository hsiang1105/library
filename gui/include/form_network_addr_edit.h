#ifndef FORM_NETWORK_ADDR_EDIT_H
#define FORM_NETWORK_ADDR_EDIT_H

#include <QWidget>

class QLineEdit;
class QLabel;
class FormNetworkAddrEditData;

class FormNetworkAddrEdit : public QWidget
{
    Q_OBJECT

public:
    FormNetworkAddrEdit(QWidget *parent = 0);
    ~FormNetworkAddrEdit();

    enum AddressType
    {
        E_IPV4,
        E_IPV6,
        E_MAC,
        E_CUSTOM
    };

    AddressType Type() const;
    QString Text();

    void SetType(AddressType type = E_IPV4);
    void SetText(const QString &text);
    void SetFrame(bool enabled);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    static bool IsValid(const QString &text, AddressType type = E_IPV4);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void preProcess();
    void postProcess();
    void clearUi();
    void initUi();
    void moveNextLineEdit(int index);
    void movePrevLineEdit(int index);
    void fixUpText(QString &text);
    QLineEdit *createLineEdit();
    QLabel *createLabel();

private:
    FormNetworkAddrEditData *data_;

private slots:
    void slotTextChanged(const QString &text);
};

#endif  // FORM_NETWORK_ADDR_EDIT_H
