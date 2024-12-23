#include "countermodel.h"

CounterModel::CounterModel(QObject* parent)
    : QAbstractTableModel{parent}
{
}

int CounterModel::rowCount(const QModelIndex& parent) const
{
    return _counters.count();
}

int CounterModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant CounterModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant{};

    switch (role)
    {
    case Qt::DisplayRole:
        return _counters.at(index.row());
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return QVariant{};
}

void CounterModel::addCounter()
{
    beginInsertRows(QModelIndex(), _counters.size(), _counters.size());
    {
        std::lock_guard<std::mutex> lock(countersMutex);
        _counters.push_back(0);
    }
    endInsertRows();
}

void CounterModel::removeCounter(const int row)
{
    if (row < 0 || row >= _counters.size())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    {
        std::lock_guard<std::mutex> lock(countersMutex);
        _counters.erase(_counters.begin() + row);
    }
    endRemoveRows();
}

void CounterModel::setCounters(const QVector<int> &counters)
{
    _counters.clear();
    for (int i = 0; i < counters.size(); i++)
        _counters << counters.at(i);

    emit layoutChanged();
}

QVector<int> CounterModel::getCounters() const
{
    return _counters;
}
