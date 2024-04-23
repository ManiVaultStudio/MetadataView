#include "MetadataSortFilterProxyModel.h"

MetadataSortFilterProxyModel::MetadataSortFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{

}

bool MetadataSortFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    if (leftData.userType() == QMetaType::QDateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else {
        static const QRegularExpression emailPattern("[\\w\\.]*@[\\w\\.]*");

        QString leftString = leftData.toString();
        if (left.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(leftString);
            if (match.hasMatch())
                leftString = match.captured(0);
        }
        QString rightString = rightData.toString();
        if (right.column() == 1) {
            const QRegularExpressionMatch match = emailPattern.match(rightString);
            if (match.hasMatch())
                rightString = match.captured(0);
        }

        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
}

bool MetadataSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    bool accept = true;
    for (int col = 0; col < sourceModel()->columnCount(); col++)
    {
        QString columnName = sourceModel()->headerData(col, Qt::Orientation::Horizontal, Qt::DisplayRole).toString();

        if (_filters.find(columnName) != _filters.end())
        {
            QString val = sourceModel()->data(sourceModel()->index(sourceRow, col, sourceParent)).toString();
            accept &= _filters.at(columnName)->acceptValue(val);
        }
    }
    return accept;
}
