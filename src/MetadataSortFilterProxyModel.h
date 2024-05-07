#pragma once

#include <QSortFilterProxyModel>

#include <QDate>
#include <QString>

#include <unordered_map>

class Filter
{
public:
    Filter(QString columnName) :
        _columnName(columnName)
    {

    }

    virtual bool acceptValue(QString val) = 0;

public:
    QString _columnName;
};

class FloatFilter : public Filter
{
public:
    FloatFilter(QString columnName) :
        Filter(columnName),
        minValue(0),
        maxValue(1)
    {

    }

    virtual bool acceptValue(QString val) override
    {
        float f = val.toFloat();

        return f > minValue && f < maxValue;
    }

    float minValue;
    float maxValue;
};

class MetadataSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    MetadataSortFilterProxyModel(QObject* parent = nullptr);

    void refresh() { invalidateFilter(); }

    std::unordered_map<QString, Filter*>& getFilters() { return _filters; }
    void addFilter(QString columnName, Filter* filter) { _filters[columnName] = filter; invalidateFilter(); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    std::unordered_map<QString, Filter*> _filters;
};
