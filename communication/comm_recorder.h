#ifndef COMM_RECORDER_H
#define COMM_RECORDER_H

#include <QObject>

class QFile;

namespace Communication {
class Recorder : public QObject
{
    Q_OBJECT
public:
    explicit Recorder(QObject *parent = 0);
    ~Recorder();

private:
    void split();

private:
    QFile *file_;
    bool is_valid_;
    int index_;

signals:
    void SigFinish();

public slots:
    void Start();
    void AppendData(const QString &text);
};
}  // namespace Communication

#endif  // COMM_RECORDER_H
