#pragma once

#include "widgets/WebWidget.h"

#include <QVariantList>

Q_DECLARE_METATYPE(QVariantList)

class MetadataView;

class FilterCommunicationObject : public mv::gui::WebCommunicationObject
{
    Q_OBJECT
public:
    FilterCommunicationObject();

signals:
    // Signals from Qt to JS side
    void setFilterInJS(const QVariantList& data);
    void setHeaderOptions(const QVariantList& data);

    // Signals triggered from JS side
    void onFilterRangeChanged(float minVal, float maxVal);
    void onHeaderOptionsChecked(QStringList checkedHeaders);

public slots:
    // Invoked from JS side 
    void onJsFilterChanged(const QVariantList& data);
    void onJsHeaderOptionsChecked(const QVariantList& data);
};

class FilterView : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    FilterView(MetadataView* plugin);

    void setHeaderOptions(const std::vector<QString>& headerOptions);

    FilterCommunicationObject& getCommunicationObject() { return _commObject; };

signals:
    void webPageLoaded();

private slots:
    void onWebPageFullyLoaded();

private:
    MetadataView*                       _plugin;            // Pointer to the main plugin class
    FilterCommunicationObject           _commObject;        // Communication Object between Qt (cpp) and JavaScript
};
