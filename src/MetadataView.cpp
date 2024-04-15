#include "MetadataView.h"

#include <AnalysisPlugin.h>
#include <event/Event.h>

#include <actions/TriggerAction.h>
#include <actions/DatasetPickerAction.h>

#include <DatasetsMimeData.h>

#include <QDebug>
#include <QMimeData>

#include <chrono>

Q_PLUGIN_METADATA(IID "studio.manivault.MetadataView")

using namespace mv;

MetadataView::MetadataView(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dropWidget(nullptr),
    _points(),
    _currentDatasetName(),
    _tableModel(nullptr),
    _tableView(nullptr)
{

}

void MetadataView::init()
{
    // Create layout
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _tableModel = new TableModel(&this->getWidget());
    _tableModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Beep"));
    _tableModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Boop"));
    _tableModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Bep"));

    _tableView = new QTableView(&this->getWidget());

    _tableModel->insertColumns(0, 5);
    _tableModel->insertRows(0, 3);

    _tableView->setModel(_tableModel);

    //// Make 10 table items
    //_geneTableItems.resize(10, nullptr);
    //_diffTableItems.resize(10, nullptr);
    //for (int i = 0; i < 10; i++)
    //{
    //    _geneTableItems[i] = new QTableWidgetItem("");
    //    _diffTableItems[i] = new QTableWidgetItem("");
    //    _tableWidget->setItem(i, 0, _geneTableItems[i]);
    //    _tableWidget->setItem(i, 1, _diffTableItems[i]);
    //}

    //QStringList columnHeaders;
    //columnHeaders.append("Gene");
    //columnHeaders.append("Mean");
    //columnHeaders.append("Std");
    //_tableView->setHorizontalHeaderLabels(columnHeaders);

    layout->addWidget(_tableView);

    // Apply the layout
    getWidget().setLayout(layout);

    // Respond when the name of the dataset in the dataset reference changes
    connect(&_points, &Dataset<Points>::guiNameChanged, this, [this]() {

        auto newDatasetName = _points->getGuiName();

        // Only show the drop indicator when nothing is loaded in the dataset reference
        _dropWidget->setShowDropIndicator(newDatasetName.isEmpty());
    });

    // Alternatively, classes which derive from hdsp::EventListener (all plugins do) can also respond to events
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));

    _eventListener.registerDataEventByType(PointType, std::bind(&MetadataView::onDataEvent, this, std::placeholders::_1));
}

void MetadataView::onDataEvent(mv::DatasetEvent* dataEvent)
{
    // Get smart pointer to dataset that changed
    const auto changedDataSet = dataEvent->getDataset();

    // Get GUI name of the dataset that changed
    const auto datasetGuiName = changedDataSet->getGuiName();

    // The data event has a type so that we know what type of data event occurred (e.g. data added, changed, removed, renamed, selection changes)
    switch (dataEvent->getType()) {

        // A points dataset was added
        case EventType::DatasetAdded:
        {
            // Cast the data event to a data added event
            const auto dataAddedEvent = static_cast<DatasetAddedEvent*>(dataEvent);

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was added";

            break;
        }

        default:
            break;
    }
}

ViewPlugin* MetadataViewFactory::produce()
{
    return new MetadataView(this);
}

mv::DataTypes MetadataViewFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This example analysis plugin is compatible with points datasets
    supportedTypes.append(PointType);

    return supportedTypes;
}

mv::gui::PluginTriggerActions MetadataViewFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getPluginInstance = [this]() -> MetadataView* {
        return dynamic_cast<MetadataView*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets >= 1 && PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<MetadataViewFactory*>(this), this, "MetadataView", "View metadata", getIcon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance();
        });

        pluginTriggerActions << pluginTriggerAction;
    }

    return pluginTriggerActions;
}
