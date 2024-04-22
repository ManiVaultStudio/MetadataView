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

    if (_mode == TableModel::Mode::SELECTION)
        return _viewIndices.size();
    else
        return _data->getNumRows();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (!_data.isValid())
            return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
        
        int rowIndex = _mode == TableModel::Mode::SELECTION ? _viewIndices[index.row()] : index.row();

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
                return QString("Cell ID");
            case 1:
                return QString("Cell Name");
            case 2:
                return QString("Subclass");
            case 3:
                return QString("Cluster");
            case 4:
                return QString("Paradigm");
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

    updateModel();
}

void TableModel::updateModel()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}
