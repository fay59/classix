//
// main.js
// Classix
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is part of Classix.
//
// Classix is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// Classix is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Classix. If not, see http://www.gnu.org/licenses/.
//

function lpad(string, length, padding)
{
	while (string.length < length)
		string = padding + string;
	return string;
}

function GoToLabel(label)
{
	var id = "#" + label;
	if (document.querySelector(id) == null)
	{
		DehighlightPC();
		var docId = document.querySelector("html").getAttribute("data-document-id");
		document.location = "cxdb://disassembly/" + docId + "/" + label;
	}
	else
	{
		document.location.hash = "#" + label;
	}
}

function DehighlightPC()
{
	var pcRows = document.querySelectorAll(".current");
	for (var i = 0; i < pcRows.length; i++)
	{
		var pcRow = pcRows[i];
		var arrowTd = pcRow.childNodes[1];
		arrowTd.removeChild(arrowTd.firstChild);
		pcRow.classList.remove("current");
		pcRow.classList.remove("error");
		
		var errorSpan = pcRow.querySelector(".error-message");
		if (errorSpan != null)
			errorSpan.parentNode.removeChild(errorSpan);
	}
}

function HighlightPC(trace, errorMessage)
{
	DehighlightPC();
	
	for (var i = 0; i < trace.length; i++)
	{
		var pc = trace[i];
		var pcString = lpad(pc.toString(16), 8, '0');
		var instructionLabel = "#i" + pcString;
		var pcRow = document.querySelector(instructionLabel);
		if (pcRow != null)
		{
			pcRow.classList.add("current");
			if (i == 0)
			{
				var img = document.createElement("img");
				img.alt = ">";
				img.src = "cxdb://resource/execution-cursor.png";
				pcRow.childNodes[1].appendChild(img);
			}
			
			if (errorMessage != null)
			{
				pcRow.classList.add("error");
				var lastTd = pcRow.querySelector("td:last-child");
				var errorSpan = document.createElement("span");
				errorSpan.className = "error-message";
				errorSpan.textContent = errorMessage;
				lastTd.appendChild(errorSpan);
			}
		}
	}
}

function BeginShowPreview(element)
{
	function CreateValueRow(title, value)
	{
		var tr = document.createElement("tr");
		var titleTd = document.createElement("td");
		titleTd.textContent = title;
		var valueTd = document.createElement("td");
		valueTd.textContent = value;
		
		tr.appendChild(titleTd);
		tr.appendChild(valueTd);
		return tr;
	}
	
	function Offset(which, element)
	{
		var totalOffset = 0;
		while (element != null)
		{
			totalOffset += element["offset" + which];
			element = element.offsetParent;
		}
		return totalOffset;
	}
	
	function IsAncestor(ancestorNode, childNode)
	{
		var parent = childNode.parentElement;
		while (parent != null)
		{
			if (parent == ancestorNode)
				return true;
			parent = parent.parentElement;
		}
		return false;
	}
	
	var timeout = setTimeout(function()
	{
		var classes = ["gpr", "fpr", "spr", "cr"];
		var values = undefined;
		for (var i = 0; i < classes.length; i++)
		{
			var className = classes[i];
			if (element.classList.contains(className))
			{
				var number = parseInt(element.textContent.replace(/[^0-9]/g, ''));
				var method = "representationsOf" + className.toUpperCase() + "_";
				var objcValues = cxdb[method](number);
				values = JSON.parse(cxdb.jsonize_(objcValues));
				break;
			}
		}
		
		if (values == undefined && element.classList.contains("ptr"))
		{
			var parts = element.textContent.match(/^(-*[0-9]+)\(r([0-9]+)\)$/);
			var register = parseInt(parts[2]);
			var registerRep = cxdb.representationsOfGPR_(register)
			var registerStringValue = JSON.parse(cxdb.jsonize_(registerRep))["unsigned"];
			var registerValue = parseInt(registerStringValue.substr(2), 16);
			var address = registerValue + parseInt(parts[1]);
			var objcValues = cxdb.representationsOfMemoryAddress_(address);
			values = JSON.parse(cxdb.jsonize_(objcValues));
			values["r" + register] = registerStringValue;
			values["address"] = "0x" + lpad(address.toString(16), 8, '0');
		}
		
		if (values == undefined)
			return;
		
		element.classList.add("active");
		
		var popoverDiv = document.createElement("div");
		popoverDiv.style.top = Offset("Top", element) - 20 + "px";
		popoverDiv.style.left = Offset("Left", element) - 25 + "px";
		popoverDiv.className = "popover";
		
		var onMouseOut = function(event)
		{
			var target = event.relatedTarget;
			if (target != popoverDiv && !IsAncestor(popoverDiv, target))
			{
				popoverDiv.parentNode.removeChild(popoverDiv);
				element.classList.remove("active");
				document.removeEventListener("mouseout", onMouseOut);
			}
		}
		document.addEventListener("mouseout", onMouseOut);
		
		var popover = document.createElement("table");
		popover.style.marginTop = element.offsetHeight + "px";
		popoverDiv.appendChild(popover);
		
		var possibleKeys = ["address", "condition", "signed", "unsigned", "float", "double", "pointer", "*unsigned", "*signed", "*float", "*double", "char*"];
		for (var i = 0; i < 32; i++) possibleKeys.unshift("r" + i);
		for (var i = 0; i < possibleKeys.length; i++)
		{
			var key = possibleKeys[i];
			if (values[key] != undefined)
				popover.appendChild(CreateValueRow(key, values[key]));
		}
		document.body.appendChild(popoverDiv);
	}, 750);
	
	var onMouseOut = function()
	{
		clearTimeout(timeout);
		element.removeEventListener("mouseout", onMouseOut);
	}
	
	element.addEventListener("mouseout", onMouseOut);
}

document.addEventListener("DOMContentLoaded", function()
{
	setTimeout(function()
	{
		var breakpoints = cxdb.breakpoints();
		for (var i = 0; i < breakpoints.length; i++)
		{
			var address = breakpoints[i];
			var selector = "#i" + lpad(address.toString(16), 8, '0');
			var tr = document.querySelector(selector);
			if (tr != null)
				tr.classList.add("breakpoint");
		}
	}, 0);
});

document.addEventListener("mouseover", function(event)
{
	var target = event.target;
	var classes = ["gpr", "fpr", "spr", "cr", "ptr"];
	for (var i = 0; i < classes.length; i++)
	{
		if (target.classList.contains(classes[i]))
		{
			BeginShowPreview(target);
		}
	}
});

document.addEventListener("click", function(event)
{
	var target = event.target;
	if (target.nodeName == "td" && target.previousSibling == null)
	{
		var tr = target.parentNode;
		var address = parseInt(tr.id.substr(1), 16);
		var added = cxdb.toggleBreakpoint_(address);
		if (added)
			tr.classList.add("breakpoint");
		else
			tr.classList.remove("breakpoint");
		
		event.preventDefault();
		event.stopPropagation();
		return;
	}
	else if (target.nodeName == "span")
	{
		var selectable = /(r|fr|sr|cr)[0-9]{1,2}/;
		var matches = target.getAttribute("class").match(selectable);
		if (matches != null && matches.length > 0)
		{
			document.body.className = "selected-" + matches[0];
			event.preventDefault();
			event.stopPropagation();
			return;
		}
	}
	document.body.className = "";
});

document.addEventListener("keydown", function(event)
{
	if (event.which == 13)
	{
		event.target.blur();
		event.stopPropagation();
		event.preventDefault();
	}
});

document.addEventListener("change", function(event)
{
	var target = event.target;
	if (target.classList.contains("func-name"))
	{
		cxdb.setDisplayName_ofLabel_(target.value, target.id);
	}
});