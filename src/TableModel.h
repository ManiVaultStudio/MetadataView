#pragma once

#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
public:
    TableModel(QObject* parent = nullptr);

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    // Already implemented by QAbstractTableModel
    //QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    // Already implemented by QAbstractTableModel
    //QModelIndex parent(const QModelIndex& index) const override;
private:

};
