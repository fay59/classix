var CXDB = function(documentId)
{
	function DoJSONQuery(path, callback)
	{
		var xhr = new XMLHttpRequest();
		xhr.open("GET", "cxdb://debugger:" + documentId + "/" + path);
		
		if (callback != undefined)
		{
			xhr.onreadystatechange = function()
			{
				if (xhr.readyState != 4) return;
				callback(JSON.parse(xhr.responseText));
			}
		}
		
		xhr.send();
	}
	
	function DoDataQuery(path, callback)
	{
		var xhr = new XMLHttpRequest();
		xhr.open("GET", "cxdb://debugger:" + documentId + "/" + path);
		
		if (callback != undefined)
		{
			xhr.onreadystatechange = function()
			{
				if (xhr.readyState != 4) return;
				callback(xhr.responseBody);
			}
		}
		
		xhr.send();
	}
	
	this.OnInstructionPointerChanged = null;
	
	this.Status = function(cb)
	{
		DoJSONQuery("status", cb);
	}
	
	this.GetGPR = function(r, cb)
	{
		DoJSONQuery("gpr/" + r, cb);
	}
	
	this.GetFPR = function(r, cb)
	{
		DoJSONQuery("fpr/" + r, cb);
	}
	
	this.GetSPR = function(r, cb)
	{
		DoJSONQuery("spr/" + r, cb);
	}
	
	this.GetCR = function(r, cb)
	{
		DoJSONQuery("cr/" + r, cb);
	}
	
	this.ReadMemory = function(address, length, cb)
	{
		DoDataQuery("memory/" + address + "/" + length, cb);
	}
	
	this.ReadByte = function(address, cb)
	{
		this.Dereference(address, 1, function(data)
		{
			cb(new Uint8Array(data)[1]);
		});
	}
	
	this.ReadHalfWord = function(address, cb)
	{
		this.Dereference(address, 2, function(data)
		{
			cb(new Uint16Array(data)[0]);
		});
	}
	
	this.ReadWord = function(address, cb)
	{
		this.Dereference(address, 4, function(data)
		{
			cb(new Uint32Array(data)[0]);
		});
	}
	
	this._InstructionPointerChanged = function(eipInfo)
	{
		// eipInfo: {section, address, reason}
		if (this.OnInstructionPointerChanged != null)
			this.OnInstructionPointerChanged.call(this, eipInfo);
	}
}