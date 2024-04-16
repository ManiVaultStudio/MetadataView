#include "TableModel.h"

TableModel::TableModel(QObject* parent) :
    QAbstractTableModel(parent),
    _data(nullptr),
    _viewIndices(0)
{

}

void TableModel::setData(mv::Dataset<Text> data)
{
    _data = data;

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

    return _viewIndices.size();
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
        {
            int rowIndex = _viewIndices[index.row()];
            switch (index.column())
            {
            case 0:
                return _data->getColumn("cell_id")[rowIndex];
            case 1:
                return _data->getColumn("cell_name_label")[rowIndex];
            case 2:
                return _data->getColumn("tree_subclass")[rowIndex];
            case 3:
                return _data->getColumn("tree_cluster")[rowIndex];
            case 4:
                return _data->getColumn("paradigm")[rowIndex];
            }
        }
        else
            return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (orientation == Qt::Horizontal)
        {
            switch (section) {
            case 0:
                return QString("cell_id");
            case 1:
                return QString("tree_class");
            case 2:
                return QString("tree_subclass");
            case 3:
                return QString("tree_cluster");
            case 4:
                return QString("paradigm");
            }
        }
        else
        {
            return section;
        }
    }
    return QVariant();
}

void TableModel::onViewIndicesChanged()
{
    _viewIndices = _data->getSelectionIndices();
    qDebug() << "Selection changed3";
    updateModel();
}

void TableModel::updateModel()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}
