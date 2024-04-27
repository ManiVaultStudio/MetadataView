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

    void onFilterRangeChanged(float minVal, float maxVal);

public slots:
    // Invoked from JS side 
    void onJsFilterChanged(const QVariantList& data);
};

class FilterView : public mv::gui::WebWidget
{
    Q_OBJECT
public:
    FilterView(MetadataView* plugin);

    FilterCommunicationObject& getCommunicationObject() { return _commObject; };

private slots:
    /** Is invoked when the js side calls js_available of the mv::gui::WebCommunicationObject (ChartCommObject)
        js_available emits notifyJsBridgeIsAvailable, which is conencted to this slot in WebWidget.cpp*/
    void initWebPage() override;

private:
    MetadataView*                       _plugin;            // Pointer to the main plugin class
    FilterCommunicationObject           _commObject;        // Communication Object between Qt (cpp) and JavaScript
};
