#pragma once

#include <QAbstractTableModel>
#include <mutex>

class CounterModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit CounterModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    void addCounter();
    void removeCounter(const int row);
    void setCounters(const QVector<int>&);
    QVector<int> getCounters() const;

private:
    QVector<int> _counters;
    std::mutex countersMutex;
};
