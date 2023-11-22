#ifndef WRITINGCLASS_H
#define WRITINGCLASS_H

#include <QObject>

class WritingClass : public QObject
{
    Q_OBJECT
public:
    explicit WritingClass(QObject *parent = nullptr);
    ~WritingClass();
    virtual void run() = 0;
    virtual void setStop() = 0;

private slots:
    virtual void readClient() = 0;
};

#endif // WRITINGCLASS_H
