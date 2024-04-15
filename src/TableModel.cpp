#include "TableModel.h"

TableModel::TableModel(QObject* parent) :
    QAbstractTableModel(parent)
{

}

int TableModel::rowCount(const QModelIndex& parent) const
{
    return 3;
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("first");
        case 1:
            return QString("second");
        case 2:
            return QString("third");
        }
    }
    return QVariant();
}
