#include "comm_recorder.h"
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "comm_parameter.h"

namespace Communication {

Recorder::Recorder(QObject *parent)
  : QObject(parent)
  , file_(NULL)
  , is_valid_(false)
  , index_(1)
{
    connect(this, &Recorder::SigFinish, this, &Recorder::deleteLater);
}

Recorder::~Recorder()
{
    if (file_) {
        delete file_;
        file_ = NULL;
    }
}

void Recorder::split()
{
    if (!file_) {
        return;
    }

    file_->close();

    QString file_path = file_->fileName();
    file_path.remove(".log");
    for (; index_ < 0x7FFFFFFF; index_++) {
        QString new_path = QString("%1(%2).log").arg(file_path).arg(index_);

        if (!QFile::exists(new_path)) {
            file_->setFileName(new_path);
            is_valid_ = file_->open(QIODevice::WriteOnly | QIODevice::Append);
            if (!is_valid_) {
                emit SigFinish();
            }
            return;
        }
    }
}

void Recorder::Start()
{
    QString dir_path = qApp->applicationDirPath() + "/recorder";
    QDir dir(dir_path);
    if (!dir.exists()) {
        dir.mkpath(dir_path);
    }

    QString text = QDateTime::currentDateTime().toString("yyyyMMdd_HH-mm-ss");
    QString file_name = QString("%1.log").arg(text);
    QString file_path = dir.absoluteFilePath(file_name);

    file_ = new QFile;
    file_->setFileName(file_path);
    is_valid_ = file_->open(QIODevice::WriteOnly | QIODevice::Append);
    if (!is_valid_) {
        emit SigFinish();
        return;
    }
}

void Recorder::AppendData(const QString &text)
{
    // when file is over 1.5MB, split it!
    if (file_ && (file_->size() > 0x180000)) {
        split();
    }

    if (!is_valid_) {
        return;
    }

    QString msg;
    if (text.startsWith("@")) {
        QString format("[yyyy-MM-dd HH:mm:ss:zzz]");
        QString time = QDateTime::currentDateTime().toString(format);
        msg = QString("%1 %2\r\n").arg(text).arg(time);
    } else {
        msg = text;
    }

    QTextStream stream(file_);
    stream << msg;
    stream.flush();
}
}  // namespace Communication
