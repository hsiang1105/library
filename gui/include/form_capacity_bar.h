#ifndef FORM_CAPACITY_BAR_H
#define FORM_CAPACITY_BAR_H

#include <QMap>
#include <QProgressBar>

class FormCapacityBarData;

class FormCapacityBar : public QProgressBar
{
    Q_OBJECT
public:
    explicit FormCapacityBar(QWidget *parent = 0);
    ~FormCapacityBar();

    enum Level
    {
        E_NORMAL,
        E_WARNING,
        E_CRITICAL,
    };

    QColor BarColor(Level level) const;
    int WarningPercent() const;
    int CriticalPercent() const;

    void SetBarColor(const QColor &color, Level level);
    void SetWarningPercent(int percent);
    void SetCriticalPercent(int percent);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    FormCapacityBarData *data_;
};

#endif  // FORM_CAPACITY_BAR_H
