#ifndef FORM_LOADING_SPINNER_H
#define FORM_LOADING_SPINNER_H

#include <QWidget>

class QTimer;
class FormLoadingSpinnerData;

class FormLoadingSpinner : public QWidget
{
    Q_OBJECT
public:
    explicit FormLoadingSpinner(QWidget *parent = 0);
    ~FormLoadingSpinner();

    bool IsAnimated() const;
    bool IsAlwaysDisplayed() const;
    int AnimationDealy() const;
    const QColor &Color() const;

    void StartAnimation();
    void StopAnimation();
    void SetAlwaysDisplay(bool enabled);
    void SetAnimationDelay(int delay);
    void SetColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FormLoadingSpinnerData *data_;

private slots:
    void slotTimeout();
};

#endif  // FORM_LOADING_SPINNER_H
