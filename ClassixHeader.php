<?php

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
	
	$header = makeHeader(basename($codeFile), "Classix", "Félix Cloutier", 2012);
	file_put_contents($codeFile, $header . join("", $file));
}

?>