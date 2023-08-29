#ifndef FORM_FADING_BUTTON_H
#define FORM_FADING_BUTTON_H

#include <QAbstractButton>

class FormFadingButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(float fader READ Fader WRITE SetFader)

public:
    explicit FormFadingButton(QWidget *parent = 0);

    float Fader() const;
    void SetFader(float value);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private:
    QPixmap contentPixmap(const QSize &size, bool checked);

private:
    float fader_;
    QPixmap pix_checked_;
    QPixmap pix_unchecked_;    
};

#endif  // FORM_FADING_BUTTON_H
