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
    _tableModel(nullptr),
    _tableView(nullptr),
    _selectionModeButton(nullptr)
{

}

void MetadataView::init()
{
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
    _tableModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Beep"));
    _tableModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Boop"));
    _tableModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Bep"));

    QList<TableModel::Header> headers;
    headers.append(TableModel::Header{ "Cell ID", "cell_id" });
    headers.append(TableModel::Header{ "Cell Name", "cell_name_label" });
    headers.append(TableModel::Header{ "Subclass", "tree_subclass" });
    headers.append(TableModel::Header{ "Cluster", "tree_cluster" });
    headers.append(TableModel::Header{ "Paradigm", "paradigm" });
    _tableModel->setHeaders(headers);

    _tableView = new QTableView(&this->getWidget());
    _tableView->setSortingEnabled(true);
    //connect(_tableModel, &QAbstractItemModel::rowsInserted, _tableView, &QTableView::update);

    _tableModel->insertColumns(0, 5);
    _tableModel->insertRows(0, 3);

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(_tableModel);
    _tableView->setModel(proxyModel);
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

    layout->addWidget(_optionAction->createWidget(&this->getWidget(), OptionAction::WidgetFlag::LineEdit));
    layout->addWidget(_tableView);
    layout->addWidget(_selectionModeButton->createWidget(&this->getWidget()));

    // Check if a text dataset already exist that contains "metadata" in the name
    for (mv::Dataset dataset : mv::data().getAllDatasets())
    {
        if (dataset->getGuiName().contains("metadata"))
        {
            _currentDataset = dataset;
            _tableModel->setData(dataset);
            _optionAction->setCustomModel(_tableModel);
        }
    }

    // Apply the layout
    getWidget().setLayout(layout);

    //// Respond when the name of the dataset in the dataset reference changes
    //connect(&_points, &Dataset<Points>::guiNameChanged, this, [this]() {

    //    auto newDatasetName = _points->getGuiName();

    //    // Only show the drop indicator when nothing is loaded in the dataset reference
    //    _dropWidget->setShowDropIndicator(newDatasetName.isEmpty());
    //});

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

            _currentDataset = changedDataSet;

            _tableModel->setData(changedDataSet);

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was added";

            break;
        }
        case EventType::DatasetDataChanged:
        {
            // Cast the data event to a data added event
            const auto dataChangedEvent = static_cast<DatasetDataChangedEvent*>(dataEvent);

            _currentDataset = changedDataSet;

            _tableModel->setData(changedDataSet);

            // Get the GUI name of the added points dataset and print to the console
            qDebug() << datasetGuiName << "was changed";

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
