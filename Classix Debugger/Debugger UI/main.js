window.onerror = function(error, file, line)
{
	debug.log_("[ERR] " + error + " at " + file + ":" + line);
}

var cxdb;
var currentAddress;
var currentAddressRow;

var ArgumentType = {
	Null: 0,
	GPR: 1,
	FPR: 2,
	SPR: 3,
	CR: 4,
	Offset: 5,
	Literal: 6
};

var SpecialPurposeRegisters = {
	1: "xer",
	8: "lr",
	9: "ctr",
	18: "dsisr",
	19: "dar",
	22: "dec",
	25: "sdr1",
	26: "srr0",
	27: "srr1",
	272: "sprg0",
	273: "sprg1",
	274: "sprg2",
	275: "sprg3",
	282: "ear",
	287: "pvr",
	528: "ibat0u",
	529: "ibat0l",
	530: "ibat1u",
	531: "ibat1l",
	532: "ibat2u",
	533: "ibat2l",
	534: "ibat3u",
	535: "ibat3l",
	536: "dbat0u",
	537: "dbat0l",
	538: "dbat1u",
	539: "dbat1l",
	540: "dbat2u",
	541: "dbat2l",
	542: "dbat3u",
	543: "dbat3l",
	1013: "dabr",
};

function CreateElement(type, attributes, children)
{
	var element = document.createElement(type);
	for (var key in attributes)
		element.setAttribute(key, attributes[key]);
	
	if (children != undefined)
	{
		for (var i = 0; i < children.length; i++)
		{
			var child = children[i];
			if (child.nodeType != undefined)
				element.appendChild(children[i]);
			else
				element.appendChild(document.createTextNode(child));
		}
	}
	return element;
}

function CreateArgumentElement(arg)
{
	function showValueBubble(rootElement, value)
	{
		var element = CreateElement("div", {"class": "value-bubble"}, [
			CreateElement("strong", {}, [rootElement.textContent]),
			": " + value + " / " + value.toString(16)
		]);
		
		// do stuff with it
	}
	
	function getValueForBubble(rootElement)
	{
		var types = [null, "GetGPR", "GetFPR", "GetSPR", "GetCR", "DereferenceWord", null];
		var type = types[arg.type];

		if (type == "DereferenceWord")
		{
			cxdb.GetGPR(arg.gpr, function(base)
			{
				cxdb.DereferenceWord(base + arg.value, showValueBubble.bind(rootElement));
			});
		}
		else if (type != null)
		{
			cxdb[type](arg.value, showValueBubble.bind(rootElement));
		}
	}
	
	function setupValueForBubble(rootElement)
	{
		var timeout;
		rootElement.addEventListener("mouseenter", function()
		{
			timeout = setTimeout(getValueForBubble.bind(rootElement), 1000);
		});
		
		rootElement.addEventListener("mouseleave", function()
		{
			clearTimeout(timeout);
		});
	}
	
	switch (arg.type)
	{
		case ArgumentType.Null:
			return document.createTextNode("(null)");
		
		case ArgumentType.GPR:
			var span = CreateElement("span", {"class": "gpr"}, ["r" + arg.value]);
			setupValueForBubble(span);
			return span;
		
		case ArgumentType.FPR:
			var span = CreateElement("span", {"class": "fpr"}, ["fr" + arg.value]);
			setupValueForBubble(span);
			return span;
		
		case ArgumentType.SPR:
			var span = CreateElement("span", {"class": "spr"},
				[arg.value in SpecialPurposeRegisters ? SpecialPurposeRegisters[arg.value] : "spr" + arg.value]);
			setupValueForBubble(span);
			return span;
		
		case ArgumentType.CR:
			var span = CreateElement("span", {"class": "cr"}, ["cr" + arg.value]);
			return span;
		
		case ArgumentType.Offset:
			var a = CreateElement("a", {href: "#", "class": "dereference"},
				[arg.value, '(', CreateArgumentElement({type: ArgumentType.GPR, value: arg.gpr}), ')']);
			return a;
		
		case ArgumentType.Literal:
			var offset = "";
			if (arg.value < 0)
			{
				offset += "-";
				arg.value = Math.abs(arg.value);
			}
			offset += "0x" + arg.value.toString(16);
			return document.createTextNode(offset);
	}
}

function ShowDisassembly(assembly)
{
	var table = document.querySelector("#disasm");
	while (table.childNodes.length > 0)
		table.removeChild(table.firstChild);
	
	function doLabel(i)
	{
		var label = assembly[i];
		if (label.instructions.length == 0)
			return;
		
		var firstLetters = label.label.substr(0, 2);
		var labelName = (firstLetters == "fn" || firstLetters == "lb") ? "." + label.label : label.label;
		var tr = CreateElement("tr", {}, [CreateElement("th", {colspan: 6, id: label.label}, [labelName])]);
		table.appendChild(tr);
		
		for (var j = 0; j < label.instructions.length; j++)
		{
			var inst = label.instructions[j];
			var argumentsTd = document.createElement("td");
			if (inst.arguments.length > 0)
			{
				var arg = CreateArgumentElement(inst.arguments[0]);
				argumentsTd.appendChild(arg);
				for (var k = 1; k < inst.arguments.length; k++)
				{
					arg = CreateArgumentElement(inst.arguments[k]);
					argumentsTd.appendChild(document.createTextNode(", "));
					argumentsTd.appendChild(arg);
				}
			}
			
			var targetTdContents = [];
			if (inst.target != null)
				targetTdContents.push(CreateElement("a", {href: "#" + inst.target}, [inst.target]));
			
			var tr = CreateElement("tr", {id: "i" + inst.location.toString(16)}, [
				CreateElement("td", {}, [inst.location.toString(16)]),
				CreateElement("td", {}, []),
				CreateElement("td", {}, [inst.code.toString(16)]),
				CreateElement("td", {}, [inst.opcode]),
				argumentsTd,
				CreateElement("td", {}, targetTdContents)
			]);
			
			if (inst.location == currentAddress)
			{
				var eip = CreateElement("img", {src: "cxdb:resource/execution-cursor.png", alt: "Current instruction"}, []);
				tr.classList.add("current");
				tr.childNodes[1].appendChild(eip);
			}
			
			table.appendChild(tr);
		}
	}
	
	var i = 0;
	function labelLoop()
	{
		if (i < assembly.length)
		{
			doLabel(i);
			i++;
			setTimeout(labelLoop, 0);
		}
	}
	
	labelLoop();
}

function UpdateStatus(status)
{
	currentAddress = status.pc;
	document.querySelector("#section").value = status.section;
	cxdb.GetSectionDisassembly(status.section, ShowDisassembly);
}

function LoadSections(sections)
{
	var sectionDropdown = document.querySelector("#section");
	
	for (var i = 0; i < sections.length; i++)
	{
		var section = CreateElement("option", {value: i}, [sections[i]]);
		sectionDropdown.appendChild(section);
	}
	
	cxdb.Status(UpdateStatus);
}

function CXDB_ShowSection(lib, sectionNumber)
{
	cxdb.GetSectionDisassembly(lib, sectionNumber, ShowDisassembly);
}

document.addEventListener("DOMContentLoaded", function()
{
	var documentId = document.querySelector("html").getAttribute("data-document-id");
	cxdb = new CXDB(documentId);
});
