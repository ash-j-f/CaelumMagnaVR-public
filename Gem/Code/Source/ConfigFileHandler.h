// An experimental class to save and load game settings using a JSON text file.
// This class is currently unused.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

/* WORK IN PROGRESS

#pragma once

#include <PLY/PLYTools.h>

#include <PLY/PLYTypes.h>

#include "PLYLog.h"

#define PLYCONF PLY::PLYConfigFileHandler::getInstance()

namespace PLY
{
	class PLYConfigFileHandler
	{

	public:
		
		static PLYConfigFileHandler *getInstance();

		void SetConfigValues(const DatabaseConnectionDetails d, const QuerySettings qs,
			const PoolSettings p, const PLYLog::LogLevel l);

		DatabaseConnectionDetails GetDatabaseConnectionDetails();
		QuerySettings GetQuerySettings();
		PoolSettings GetPoolSettings();
		PLYLog::LogLevel GetLogLevel();
		AZStd::string GetConnectionString();
	private:

		std::mutex m_configMutex;

		DatabaseConnectionDetails m_d;
		QuerySettings m_qs;
		PoolSettings m_p;
		PLYLog::LogLevel m_logLevel;

		//The database connection string.
		AZStd::string m_connectionString;

		enum CType { NONE, STRING, INT, UNSIGNED_INT, BOOL, LOGLEVEL, PRIORITY, WAITMODE, SSLMODE };

		struct ConfEntry
		{
			std::string group;
			std::string name;
			CType type;
			void *set;
		};

		void Save();

		void Load();

		void RegenConnectionString();

		template<class T> void AddConfEntry(std::string group, std::string name, T set, std::vector<ConfEntry> &vec);

		PLYConfigFileHandler();
		~PLYConfigFileHandler();
	};

}
*/