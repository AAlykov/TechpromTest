#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<QVector<int>>("QVector<int>");

    _counterModel = new CounterModel(this);
    ui->tableView->setModel(_counterModel);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView* header = ui->tableView->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->addButton, &QPushButton::clicked, _counterModel, &CounterModel::addCounter);

    connect(ui->removeButton, &QPushButton::clicked, this, [this]() {
        auto index = ui->tableView->currentIndex();
        if (index.isValid())
        {
            _counterModel->removeCounter(index.row());
        }
    });
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveToDatabase);
    connect(this, &MainWindow::countersChanged, _counterModel, &CounterModel::setCounters);

    connect(this, &MainWindow::freqChanged, this,
            [this](float freq) { ui->freqLabel->setText(QString::number(freq)); });

    setupDatabase();
    loadFromDatabase();

    startWorkerThread();
    isRunning = true;
}

void MainWindow::startWorkerThread()
{
    workerThread = std::thread([this]() {
        while (isRunning)
        {
            QVector<int> counters = _counterModel->getCounters();
            int sum = 0;

            for (auto& counter : counters)
            {
                sum += counter;
                counter++;
            }

            sumT0 = sumT1;
            sumT1 = sum;

            t1 = QDateTime::currentDateTimeUtc().toTime_t();

            float freq = (sumT1 - sumT0) / float(t1 - t0);
            freq = std::round(freq * 100.0f) / 100.0f;

            if (freq > 0)
                emit freqChanged(freq);

            t0 = t1;
            emit countersChanged(counters);

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
}

void MainWindow::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("counters.db");

    if (!db.open())
    {
        QMessageBox::critical(this, "Error", db.lastError().text());
        exit(EXIT_FAILURE);
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS counters (id INTEGER PRIMARY KEY, value INTEGER)");
}

void MainWindow::loadFromDatabase()
{
    QVector<int> counters;
    QSqlQuery query("SELECT value FROM counters");
    while (query.next())
    {
        counters << query.value(0).toInt();
    }
    _counterModel->setCounters(counters);
}

void MainWindow::saveToDatabase()
{

    QSqlQuery query;
    query.exec("DELETE FROM counters");

    QVector<int> counters = _counterModel->getCounters();

    for (const auto& counter : counters)
    {
        query.prepare("INSERT INTO counters (value) VALUES (:value)");
        query.bindValue(":value", counter);
        if (!query.exec())
        {
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
            return;
        }
    }

    QMessageBox::information(this, "Информация", "Данные сохранены");
}

MainWindow::~MainWindow()
{
    isRunning = false;

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    delete ui;
}
