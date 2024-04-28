#include "MetadataView.h"

#include "MetadataSortFilterProxyModel.h"

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

namespace
{
    bool isDatasetMetadata(mv::Dataset<DatasetImpl> dataset)
    {
        return dataset->hasProperty("PatchSeqType") && dataset->getProperty("PatchSeqType").toString() == "Metadata";
    }
}

MetadataView::MetadataView(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dropWidget(nullptr),
    _tableModel(nullptr),
    _tableView(nullptr),
    _optionAction(nullptr),
    _searchInput(nullptr),
    _filterOptions(nullptr),
    _filterView(nullptr),
    _proxyModel(nullptr),
    _selectionModeButton(nullptr)
{

}

void MetadataView::init()
{
    // 
    connect(&_currentDataset, &mv::Dataset<DatasetImpl>::changed, this, &MetadataView::onDataChanged);
    connect(&_currentDataset, &mv::Dataset<DatasetImpl>::dataChanged, this, &MetadataView::onDataChanged);

    // Create layout
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    _searchInput = new QLineEdit(&this->getWidget());

    //connect(_searchInput, &QLineEdit::textChanged, 

    _optionAction = new OptionAction(&this->getWidget(), "Search input");
    _selectionModeButton = new ToggleAction(&this->getWidget(), "Selection mode");
    connect(_selectionModeButton, &ToggleAction::toggled, this, [this](bool enabled) {
        _tableModel->setMode(enabled ? TableModel::Mode::SELECTION : TableModel::Mode::FILTER);
    });

    _tableModel = new TableModel(&this->getWidget());

    QList<QString> headers;
    headers.append("Cell ID");
    headers.append("Cell Name");
    headers.append("Subclass");
    headers.append("Cluster");
    headers.append("Paradigm");
    headers.append("Sag");
    headers.append("Num Branches (BD)");
    _tableModel->setHeaders(headers);

    _tableView = new QTableView(&this->getWidget());
    _tableView->setSortingEnabled(true);

    _proxyModel = new MetadataSortFilterProxyModel(this);
    _proxyModel->setSourceModel(_tableModel);
    _tableView->setModel(_proxyModel);
    _tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QItemSelectionModel* selectionModel = _tableView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, [this, selectionModel](const QItemSelection& selected, const QItemSelection& deselected) {
        QModelIndexList modelIndexList = selectionModel->selectedRows();
        std::vector<uint32_t> rows(modelIndexList.size());
        for (int i = 0; i < modelIndexList.size(); i++)
        {
            const QModelIndex& modelIndex = modelIndexList[i];
            rows[i] = modelIndex.row();
        }
        _currentDataset->setSelectionIndices(rows);
        events().notifyDatasetDataSelectionChanged(_currentDataset);
    });

    // Filter options
    _dimensionsModel = new QStandardItemModel(3, 1, this);
    for (int i = 0; i < 3; i++)
    {
        QStandardItem* item = new QStandardItem(QString("Item %0").arg(i));

        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);

        _dimensionsModel->setItem(i, 0, item);
    }
    _filterOptions = new QComboBox();
    _filterOptions->setModel(_dimensionsModel);

    // Filter view
    _filterView = new FilterView(this);
    _filterView->setPage(":metadata_view/filterview/filterview.html", "qrc:/metadata_view/filterview/");

    //layout->addWidget(_optionAction->createWidget(&this->getWidget(), OptionAction::WidgetFlag::LineEdit));
    layout->addWidget(_filterOptions);
    layout->addWidget(_tableView);
    layout->addWidget(_filterView);
    layout->addWidget(_selectionModeButton->createWidget(&this->getWidget()));


    // Check if a text dataset already exist that contains "metadata" in the name
    for (mv::Dataset dataset : mv::data().getAllDatasets())
    {
        if (isDatasetMetadata(dataset))
            _currentDataset = dataset;
    }

    // Apply the layout
    getWidget().setLayout(layout);

    connect(&_filterView->getCommunicationObject(), &FilterCommunicationObject::onFilterRangeChanged, this, &MetadataView::onFilterRangeChanged);

    // Alternatively, classes which derive from hdsp::EventListener (all plugins do) can also respond to events
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAdded));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));

    _eventListener.registerDataEventByType(TextType, std::bind(&MetadataView::onDataEvent, this, std::placeholders::_1));
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

            if (isDatasetMetadata(changedDataSet))
                _currentDataset = changedDataSet;

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was added";

            break;
        }
        case EventType::DatasetDataChanged:
        {
            // Cast the data event to a data added event
            const auto dataChangedEvent = static_cast<DatasetDataChangedEvent*>(dataEvent);

            if (isDatasetMetadata(changedDataSet))
                _currentDataset = changedDataSet;

            break;
        }

        // The selection of a dataset got changed
        case EventType::DatasetDataSelectionChanged:
        {
            if (!_currentDataset.isValid())
                break;

            if (changedDataSet != _currentDataset)
                break;

            _tableModel->onViewIndicesChanged();

            break;
        }

        default:
            break;
    }
}

void MetadataView::onDataChanged()
{
    qDebug() << "onDataChanged";

    _tableModel->setData(_currentDataset);

    // Get the GUI name of the added points dataset and print to the console
    qDebug() << _currentDataset->getGuiName() << "was changed";

    // Compute range of sag
    std::vector<QString> sagColumn = _currentDataset->getColumn("Sag");
    float minValue = std::numeric_limits<float>::max();
    float maxValue = -std::numeric_limits<float>::max();

    for (int i = 0; i < sagColumn.size(); i++)
    {
        QString& s = sagColumn[i];
        if (s.isEmpty())
            continue;

        float value = sagColumn[i].toFloat();
        if (value < minValue) minValue = value;
        if (value > maxValue) maxValue = value;
    }

    QVariantList vals;
    vals.append(minValue);
    vals.append(maxValue);

    emit _filterView->getCommunicationObject().setFilterInJS(vals);

    std::vector<QString> columnNames = _currentDataset->getColumnNames();
    for (int i = 0; i < columnNames.size(); i++)
    {
        QStandardItem* item = new QStandardItem(columnNames[i]);

        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);

        _dimensionsModel->setItem(i, 0, item);
    }
}

void MetadataView::onFilterRangeChanged(float minVal, float maxVal)
{
    if (_proxyModel->getFilters().find("Sag") != _proxyModel->getFilters().end())
    {
        FloatFilter* filter = dynamic_cast<FloatFilter*>(_proxyModel->getFilters().at("Sag"));
        filter->minValue = minVal;
        filter->maxValue = maxVal;

        _proxyModel->refresh();
    }
    else
    {
        // TODO Fix memory leak
        FloatFilter* sagFilter = new FloatFilter("Sag");
        sagFilter->minValue = 0.2;
        sagFilter->maxValue = 0.5;

        _proxyModel->addFilter("Sag", sagFilter);
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
