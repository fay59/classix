<?php
//
// ClassixHeader.php
//
// Copyright (C) 2012 Félix Cloutier
//
// This file is distributed with the Classix project source but is licensed
// separately under the following terms:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// 	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
// ClassixHeader.php IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// (How ironic is it that a PHP script that adds the GPL license header is
// itself licensed under the MIT license.)

function recursiveFileList($extensions, $dirPath = ".")
{
	$set = array_flip($extensions);
	$result = array();
	
	if ($dir = opendir($dirPath))
	while (($entry = readdir($dir)) !== false)
	{
		if ($entry[0] == ".")
			continue;
		
		$path = $dirPath . "/" . $entry;
		if (is_dir($path))
		{
			$result = array_merge($result, recursiveFileList($extensions, $path));
		}
		else
		{
			$ext = substr($entry, strrpos($entry, '.') + 1);
			if (isset($set[$ext]))
				$result[] = $path;
		}
	}
	return $result;
}

function makeHeader($file, $project, $author, $year)
{
return <<<END
//
// $file
// $project
//
// Copyright (C) $year $author
//
// This file is part of $project.
//
// $project is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// $project is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// $project. If not, see http://www.gnu.org/licenses/.
//


END;
}

foreach (recursiveFileList(array('h', 'c', 'cpp', 'm', 'mm', 'js', 'pch')) as $codeFile)
{
	$file = file($codeFile);
	for ($i = 0; $i < count($file); $i++)
	{
		$line = trim($file[$i]);
		if (!empty($line) && substr($line, 0, 2) != "//")
			break;
		
		unset($file[$i]);
	}
	
	$projectName = "Classix";
	$author = "Félix Cloutier";
	$year = 2012;
	
	$header = makeHeader(basename($codeFile), $projectName, $author, $year);
	file_put_contents($codeFile, $header . join("", $file));
}

?>