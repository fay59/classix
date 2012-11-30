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
	}
}

function HighlightPC(pc)
{
	function lpad(string, length, padding)
	{
		while (string.length < length)
			string = padding + string;
		return string;
	}
	
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
	}
	
	var visibleYBegin = document.body.scrollTop;
	var visibleYEnd = document.body.scrollTop + document.body.scrollHeight;
	if (pcRow.offsetTop < visibleYBegin || pcRow.offsetTop > visibleYEnd)
	{
		document.body.scrollTop = pcRow.offsetTop - 20;
	}
}

document.addEventListener("DOMContentLoaded", function()
{
	var url = document.location.toString();
	var location = url.substr(url.lastIndexOf('/') + 1);
	document.location.hash = location
});
