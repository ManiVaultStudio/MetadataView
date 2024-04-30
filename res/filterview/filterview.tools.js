// ManiVault invokes this function to set the plot data,
// when emitting qt_js_setDataInJS from the communication object
// The connection is established in qwebchannel.tools.js
function drawChart(d) {
    log("FilterViewJS: filterview.tools.js: draw chart")

    rangeMin = parseFloat(d[0]);
    rangeMax = parseFloat(d[1]);
    log(d)
    log(rangeMin)
    log(rangeMax)
    setRangeMin(rangeMin);
    setRangeMax(rangeMax);
}

function setHeaderOptions(d)
{
    log("FilterViewJS: filterview.tools.js: setHeaderOptions")
    
    selectOptions = document.querySelector('#mySelectOptions');
    while (selectOptions.firstChild) {
        selectOptions.removeChild(selectOptions.lastChild);
    }
    
    for (let i = 0; i < d.length; i++)
    {
        addMultiSelectOption(d[i]);
    }
}

function handleChange(minVal, maxVal)
{
    onJsFilterChanged([minVal, maxVal])
}
