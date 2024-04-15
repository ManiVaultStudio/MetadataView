#include "TableModel.h"

TableModel::TableModel(QObject* parent) :
    QAbstractTableModel(parent)
{

}

void TableModel::setData(mv::Dataset<Text> data)
{
    _data = data;
    qDebug() << "Data set";
    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

// Abstract Table Model Overrides
int TableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    if (!_data.isValid())
        return 0;

    return _data->getNumPoints();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (_data.isValid())
            return _data->getColumn("cell_id")[index.row()];
        else
            return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
    }

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
