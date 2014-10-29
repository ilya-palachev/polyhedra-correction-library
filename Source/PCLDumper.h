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
 * - declaration.
 */

#ifndef PCL_DUMPER
#define PCL_DUMPER

#include <fstream>
#include <string>

#include "DebugPrint.h"

/** Levels of dumping. */
enum PCLDumperLevel
{
	PCL_DUMPER_LEVEL_ZERO,	/**< Dumping is not set. */
	PCL_DUMPER_LEVEL_DEBUG,	/**< Debug level. */
	PCL_DUMPER_LEVEL_OUTPUT	/**< Output level, always dumped. */
};

/**
 * Manager of dumping of different structures used for output and
 * debug purposes
 */
class PCLDumper
{
private:
	/** Whether debug dumps are done. */
	bool ifVerbose_;

	/** Current level of dumping. */
	PCLDumperLevel level_;

	/** Current stream to be printed. */
	std::ofstream stream_;

	/** Base name for output files. */
	std::string nameBase_;

public:
	/** Empty constructor. */
	PCLDumper();

	/** Destructor. */
	~PCLDumper();

	/**
	 * Streaming operator.
	 *
	 * @param object	The object to be dumped.
	 *
	 * @return 		The dumper is not ready for printing.
	 */
	template<typename TPrintable>
	PCLDumper &operator<<(TPrintable const &object)
	{
		DEBUG_START;
		switch (level_)
		{
		case PCL_DUMPER_LEVEL_ZERO:
			ERROR_PRINT("Please specify properties of dumping");
			break;
		case PCL_DUMPER_LEVEL_DEBUG:
			if (ifVerbose_)
				stream_ << object;
			break;
		case PCL_DUMPER_LEVEL_OUTPUT:
			stream_ << object;
			break;
		}

		level_ = PCL_DUMPER_LEVEL_ZERO;
		stream_.close();
		
		DEBUG_END;
		return *this;
	}

	/**
	 * Function call operator.
	 * Initializes dumper's properties before dumping.
	 *
	 * @param level	The level of dumping.
	 * @param name	The name of dump.
	 *
	 * @return 	The dumper is ready for dumping.
	 */
	PCLDumper &operator()(PCLDumperLevel level, const std::string name);

	/** Enables verbose mode of dumper. */
	void enableVerboseMode();

	/**
	 * Sets the basename of all dumps.
	 *
	 * @param bnameBase	The basename.
	 */
	void setNameBase(std::string nameBase);
};

/** Default PCLDumper that is used inside the library. */
extern PCLDumper globalPCLDumper;

#endif /* PCL_DUMPER */
