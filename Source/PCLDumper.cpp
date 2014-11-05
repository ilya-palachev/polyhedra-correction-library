/*
 * Copyright (c) 2009-2014 Ilya Palachev <iliyapalachev@gmail.com>
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
 * @file PCLDumper.h
 * @brief Manager of dumping of different structures used for output and
 * debug purposes
 * - implementation.
 */

#include <iostream>

#include "PCLDumper.h"

PCLDumper globalPCLDumper;

PCLDumper::PCLDumper() :
	ifVerbose_(false),
	iCurrentDumpCount_(0),
	level_(PCL_DUMPER_LEVEL_ZERO),
	nameBase_(),
	nameSuffix_()
{
	DEBUG_START;
	DEBUG_END;
}

PCLDumper::~PCLDumper()
{
	DEBUG_START;
	DEBUG_END;
}

PCLDumper &PCLDumper::operator()(PCLDumperLevel level,
	const std::string nameSuffix)
{
	DEBUG_START;
	level_ = level;
	nameSuffix_ = nameSuffix;
	DEBUG_END;
	return *this;
}

void PCLDumper::enableVerboseMode()
{
	DEBUG_START;
	ifVerbose_ = true;
	DEBUG_END;
}

void PCLDumper::setNameBase(std::string nameBase)
{
	DEBUG_START;
	nameBase_ = nameBase;
	DEBUG_END;
}
