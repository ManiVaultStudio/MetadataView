#pragma once

#include <ViewPlugin.h>

#include "TableModel.h"

#include <Dataset.h>
#include <widgets/DropWidget.h>
#include <actions/OptionAction.h>

#include <PointData/PointData.h>
#include <TextData/TextData.h>

#include <QWidget>

#include <QWidget>
#include <QTableView>
#include <QStringList>

/** All plugin related classes are in the ManiVault plugin namespace */
using namespace mv::plugin;

/** Drop widget used in this plugin is located in the ManiVault gui namespace */
using namespace mv::gui;

/** Dataset reference used in this plugin is located in the ManiVault util namespace */
using namespace mv::util;

class QLabel;

class MetadataView : public ViewPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     */
    MetadataView(const PluginFactory* factory);

    /** Destructor */
    ~MetadataView() override = default;
    
    /** This function is called by the core after the view plugin has been created */
    void init() override;

    /**
     * Invoked when a data event occurs
     * @param dataEvent Data event which occurred
     */
    void onDataEvent(mv::DatasetEvent* dataEvent);

protected:
    DropWidget*             _dropWidget;                /** Widget for drag and drop behavior */

    mv::Dataset<Text>       _currentDataset;
    OptionAction*           _optionAction;
    QLineEdit*              _searchInput;
    QTableView*             _tableView;
    TableModel*             _tableModel;

    // Search GUI
    ToggleAction*           _selectionModeButton;
};

/**
 * Metadata view factory class
 *
 * Note: Factory does not need to be altered (merely responsible for generating new plugins when requested)
 */
class MetadataViewFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.MetadataView"
                      FILE  "MetadataView.json")

public:

    /** Default constructor */
    MetadataViewFactory() {}

    /** Destructor */
    ~MetadataViewFactory() override {}
    
    /** Creates an instance of the example view plugin */
    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the example view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};
