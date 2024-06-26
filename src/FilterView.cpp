#include "FilterView.h"

#include "MetadataView.h"

#include <QDebug>
#include <QString>

using namespace mv;
using namespace mv::gui;

// =============================================================================
// FilterCommunicationObject
// =============================================================================

FilterCommunicationObject::FilterCommunicationObject()
{
}

void FilterCommunicationObject::onJsFilterChanged(const QVariantList& data) {
    float minVal = data[0].toFloat();
    float maxVal = data[1].toFloat();
    
    // Notify ManiVault core and thereby other plugins about new selection
    emit onFilterRangeChanged(minVal, maxVal);
}

void FilterCommunicationObject::onJsHeaderOptionsChecked(const QVariantList& data)
{
    qDebug() << "OnJSHeaderOptionChecked";
    QStringList headerOptionsChecked;
    for (QVariant var : data)
    {
        headerOptionsChecked.push_back(var.toString());
    }
    qDebug() << headerOptionsChecked;
    emit onHeaderOptionsChecked(headerOptionsChecked);
}

// =============================================================================
// FilterView
// =============================================================================

FilterView::FilterView(MetadataView* plugin) :
    _plugin(plugin),
    _commObject()
{
    connect(this, &WebWidget::webPageFullyLoaded, this, &FilterView::onWebPageFullyLoaded);

    // For more info on drag&drop behavior, see the ExampleViewPlugin project
    setAcceptDrops(true);

    setMinimumWidth(400);
    setMinimumHeight(240);

    setMaximumHeight(240);

    // Ensure linking to the resources defined in res/example_chart.qrc
    Q_INIT_RESOURCE(metadata_resources);

    // ManiVault and Qt create a "QtBridge" object on the js side which represents _comObject
    // there, we can connect the signals qt_js_* and call the slots js_qt_* from our communication object
    init(&_commObject);

    layout()->setContentsMargins(0, 0, 0, 0);
}

void FilterView::setHeaderOptions(const std::vector<QString>& headerOptions)
{
    QVariantList data;
    for (int i = 0; i < headerOptions.size(); i++)
    {
        data.append(headerOptions[i]);
        // Enable the first 6 dimensions by default
        if (i < 6)
            data.append(QVariant(true));
        else
            data.append(QVariant(false));
    }
    
    emit _commObject.setHeaderOptions(data);
}

void FilterView::onWebPageFullyLoaded()
{
    qDebug() << "FilterView::onWebPageFullyLoaded: Web page completely loaded.";
    emit webPageLoaded();
}
