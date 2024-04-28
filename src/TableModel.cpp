#include "TableModel.h"

TableModel::TableModel(QObject* parent) :
    QAbstractTableModel(parent),
    _metadata(nullptr),
    _viewIndices(0),
    _mode(TableModel::Mode::FILTER)
{

}

void TableModel::setData(mv::Dataset<Text> data)
{
    _metadata = data;

    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}

// Abstract Table Model Overrides
int TableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    if (!_metadata.isValid())
        return 0;

    if (_mode == TableModel::Mode::SELECTION)
        return _viewIndices.size();
    else
        return _metadata->getNumRows();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    return _headers.size();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (!_metadata.isValid())
            return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
        
        int rowIndex = _mode == TableModel::Mode::SELECTION ? _viewIndices[index.row()] : index.row();

        // Get the column based on the header name of the colIndex, then access the row from that
        return _metadata->getColumn(_headers[index.column()])[rowIndex];
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section < _headers.size())
                return _headers[section];
            else
                qWarning() << "Tried to access a column index higher than the number of header columns";
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
    _viewIndices = _metadata->getSelectionIndices();

    updateModel();
}

void TableModel::updateModel()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));

    emit layoutAboutToBeChanged();
    emit layoutChanged();
}
