window.onload = (event) => {
  initMultiselect();
};

function initMultiselect() {
  checkboxStatusChange();

  document.addEventListener("click", function(evt) {
    var flyoutElement = document.getElementById('myMultiselect'),
      targetElement = evt.target; // clicked element

    do {
      if (targetElement == flyoutElement) {
        // This is a click inside. Do nothing, just return.
        //console.log('click inside');
        return;
      }

      // Go up the DOM
      targetElement = targetElement.parentNode;
    } while (targetElement);

    // This is a click outside.
    toggleCheckboxArea(true);
    //console.log('click outside');
  });
  
  addMultiSelectOption("Help");
}

function addMultiSelectOption(optionName, checked)
{
    selectOptions = document.querySelector('#mySelectOptions');
    // <label for="one"><input type="checkbox" id="one" onchange="checkboxStatusChange()" value="one" /> First checkbox</label>
    label = document.createElement("label");
    label.setAttribute("for", optionName);
    input = document.createElement("input");
    input.type = "checkbox";
    input.id = optionName;
    input.setAttribute("onchange", "checkboxStatusChange()");
    input.value = optionName;
    input.checked = checked;
    
    t = document.createTextNode(" " + optionName)
    label.appendChild(input);
    label.appendChild(t);

    document.getElementById("mySelectOptions").appendChild(label);
}

function checkboxStatusChange()
{
  log("Checkbox status change()")
  var multiselect = document.getElementById("mySelectLabel");
  var multiselectOption = multiselect.getElementsByTagName('option')[0];

  var values = [];
  var checkboxes = document.getElementById("mySelectOptions");
  var checkedCheckboxes = checkboxes.querySelectorAll('input[type=checkbox]:checked');

  for (const item of checkedCheckboxes) {
    var checkboxValue = item.getAttribute('value');
    values.push(checkboxValue);
  }

  var dropdownValue = "Nothing is selected";
  if (values.length > 0) {
    dropdownValue = values.join(', ');
  }

  onHeaderOptionChecked(values)

  multiselectOption.innerText = dropdownValue;
}

function toggleCheckboxArea(onlyHide = false) {
  var checkboxes = document.getElementById("mySelectOptions");
  var displayValue = checkboxes.style.display;

  if (displayValue != "block") {
    if (onlyHide == false) {
      checkboxes.style.display = "block";
    }
  } else {
    checkboxes.style.display = "none";
  }
}
