/*
 * Copyright (c) 2009-2016 Ilya Palachev <iliyapalachev@gmail.com>
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
#include <sstream>
#include <iomanip>
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

	/** The number of the current dump. */
	int iCurrentDumpCount_;

	/** Current level of dumping. */
	PCLDumperLevel level_;

	/** Base name for output files. */
	std::string nameBase_;

	/** Suffix name for output file. */
	std::string nameSuffix_;

public:
	/** Empty constructor. */
	PCLDumper();

	/** Destructor. */
	~PCLDumper();

	/**
	 * Streaming operator.
	 *
	 * @param dumper	The dumper.
	 * @param object	The object to be dumped.
	 *
	 * @return 		The dumper is not ready for printing.
	 */
	template<typename TPrintable>
	friend PCLDumper &operator<<(PCLDumper &dumper,
		TPrintable object)
	{
		DEBUG_START;
		std::ofstream stream;
		std::stringstream count;
		count << std::setfill('0') << std::setw(3)
			<< dumper.iCurrentDumpCount_;
		std::string filename = dumper.nameBase_ + "."
			+ count.str() + "." + dumper.nameSuffix_;
		DEBUG_PRINT("file name = %s", filename.c_str());
		stream.open(filename, std::ofstream::out);

		switch (dumper.level_)
		{
		case PCL_DUMPER_LEVEL_ZERO:
			DEBUG_PRINT("PCL_DUMPER_LEVEL_ZERO");
			ERROR_PRINT("Please specify parameters!");
			break;
		case PCL_DUMPER_LEVEL_DEBUG:
			DEBUG_PRINT("PCL_DUMPER_LEVEL_DEBUG");
			if (dumper.ifVerbose_)
			{
				std::cerr << "Dumping to file " << filename
					<< std::endl;
				stream << object;
				std::cerr << "...dumping done" << std::endl;
			}
			break;
		case PCL_DUMPER_LEVEL_OUTPUT:
			DEBUG_PRINT("PCL_DUMPER_LEVEL_OUTPUT");
			std::cerr << "Dumping to file " << filename
				<< std::endl;
			stream << object;
			std::cerr << "...dumping done" << std::endl;
			break;
		default:
			break;
		}

		dumper.level_ = PCL_DUMPER_LEVEL_ZERO;
		stream.close();

		++dumper.iCurrentDumpCount_;

		DEBUG_END;
		return dumper;
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
