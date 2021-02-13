/*
 * Copyright (c) 2009-2017 Ilya Palachev <iliyapalachev@gmail.com>
 *
 * This file is part of Polyhedra Correction Library.
 *
 * Polyhedra Correction Library is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Polyhedra Correction Library is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyhedra Correction Library.
 * If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file DefaultScaner.h
 * @brief Default scaner used for different input file formats
 */

#ifndef DEFAULT_SCANER_H
#define DEFAULT_SCANER_H
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

class DefaultScaner
{
	const unsigned maxLineLength = 1024;
	const char *path;
	FILE *fd;
	char *line;

public:
	DefaultScaner(const char *path) : path(path), fd(nullptr), line(nullptr)
	{
	}

	~DefaultScaner()
	{
		if (fd)
			fclose(fd);
		if (line)
			free(line);
	}

	bool open()
	{
		fd = (FILE *)fopen(path, "r");
		return (fd != nullptr);
	}

	char *getline()
	{
		if (!fd)
			return nullptr;
		if (line)
			free(line);
		line = (char *)malloc(maxLineLength * sizeof(char));
		while (fgets(line, maxLineLength, fd))
			if (line[0] != '#' && strlen(line) > 0)
			{
				char *l = line;
				bool onlySpaces = true;
				while (*l != '\0')
				{
					if (!isspace(*l))
						onlySpaces = false;
					++l;
				}

				if (!onlySpaces)
					return line;
			}
		return nullptr;
	}
};

#endif /* DEFAULT_SCANER_H */
