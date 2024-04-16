#pragma once

#include <QAbstractTableModel>

#include <TextData/TextData.h>

class TableModel : public QAbstractTableModel
{
public:
    TableModel(QObject* parent = nullptr);

    void setData(mv::Dataset<Text> data);

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

public slots:
    void onViewIndicesChanged();

private:
    void updateModel();

    // Already implemented by QAbstractTableModel
    //QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    // Already implemented by QAbstractTableModel
    //QModelIndex parent(const QModelIndex& index) const override;

private:
    mv::Dataset<Text> _data;

    std::vector<uint32_t> _viewIndices;
};
