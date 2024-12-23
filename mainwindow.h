#pragma once

#include "countermodel.h"

#include <QDateTime>
#include <QDebug>
#include <QMainWindow>
#include <QMessageBox>
#include <QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <thread>

QT_BEGIN_NAMESPACE

namespace Ui
{
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void countersChanged(QVector<int>);
    void freqChanged(float);

private:
    Ui::MainWindow* ui;

    CounterModel* _counterModel;

    std::thread workerThread;

    uint t0{0};
    uint t1{0};
    int sumT0{0};
    int sumT1{0};

    std::atomic<bool> isRunning;
    void startWorkerThread();

    QSqlDatabase db;
    void setupDatabase();
    void loadFromDatabase();
    void saveToDatabase();
};
