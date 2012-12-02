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
	var pcRow = document.querySelector(".current");
	if (pcRow != null)
	{
		var arrowTd = pcRow.childNodes[1];
		arrowTd.removeChild(arrowTd.firstChild);
		pcRow.classList.remove("current");
		pcRow.classList.remove("error");
		
		var errorSpan = pcRow.querySelector(".error-message");
		if (errorSpan != null)
			errorSpan.parentNode.removeChild(errorSpan);
	}
}

function HighlightPC(pc, errorMessage)
{
	DehighlightPC();
	
	var pcString = lpad(pc.toString(16), 8, '0');
	var instructionLabel = "#i" + pcString;
	var pcRow = document.querySelector(instructionLabel);
	if (pcRow != null)
	{
		pcRow.classList.add("current");
		var img = document.createElement("img");
		img.src = "cxdb://resource/execution-cursor.png";
		pcRow.childNodes[1].appendChild(img);
		
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

document.addEventListener("DOMContentLoaded", function()
{
	var url = document.location.toString();
	var location = url.substr(url.lastIndexOf('/') + 1);
	document.location.hash = location;
	
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
	}
});