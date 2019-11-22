// An experimental class to save and load game settings using a JSON text file.
// This class is currently unused.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

/* WORK IN PROGRESS

#include <AzCore/IO/FileIO.h>

#include <AzCore/JSON/rapidjson.h>
#include <AzCore/JSON/document.h>
#include <AzCore/JSON/pointer.h>
#include <AzCore/JSON/stringbuffer.h>
#include <AzCore/JSON/writer.h>
#include <AzCore/JSON/prettywriter.h>

#include <PLY/PLYConfigFileHandler.h>

using namespace PLY;

PLYConfigFileHandler::PLYConfigFileHandler()
{
	m_logLevel = PLYLOG_GET_DEFAULT_LEVEL;
	Load();
	RegenConnectionString();
}

PLYConfigFileHandler::~PLYConfigFileHandler()
{

}

PLYConfigFileHandler *PLYConfigFileHandler::getInstance()
{
	static PLYConfigFileHandler instance;
	return &instance;
}

void PLYConfigFileHandler::Save()
{

	unsigned int jsonSize = 0;

	const void *data;
	
	using namespace rapidjson;

	Document d;

	StringBuffer buffer;

	Pointer("/DatabaseSettings/host").Set(d, m_d.host.c_str());
	Pointer("/DatabaseSettings/port").Set(d, m_d.port);
	Pointer("/DatabaseSettings/sslMode").Set(d, m_d.sslMode);
	Pointer("/DatabaseSettings/database").Set(d, m_d.database.c_str());
	Pointer("/DatabaseSettings/username").Set(d, m_d.username.c_str());
	Pointer("/DatabaseSettings/password").Set(d, m_d.password.c_str());
	Pointer("/DatabaseSettings/connectTimeout").Set(d, m_d.connectTimeout);
	Pointer("/DatabaseSettings/reconnectWaitTime").Set(d, m_d.reconnectWaitTime);

	Pointer("/QuerySettings/queryTTL").Set(d, m_qs.queryTTL);
	Pointer("/QuerySettings/resultTTL").Set(d, m_qs.resultTTL);
	Pointer("/QuerySettings/useTransaction").Set(d, m_qs.useTransaction);
	Pointer("/QuerySettings/advertiseResult").Set(d, m_qs.advertiseResult);

	Pointer("/PoolSettings/managerPriority").Set(d, m_p.managerPriority);
	Pointer("/PoolSettings/workerPriority").Set(d, m_p.workerPriority);
	Pointer("/PoolSettings/waitMode").Set(d, m_p.waitMode);
	Pointer("/PoolSettings/minPoolSize").Set(d, m_p.minPoolSize);
	Pointer("/PoolSettings/maxPoolSize").Set(d, m_p.maxPoolSize);

	Pointer("/Log/logLevel").Set(d, m_logLevel);

	//Use "PrettyWriter" as opposed to "Writer" here so output is nicely indented and spaced for human readability.
	PrettyWriter<StringBuffer> writer(buffer);
	d.Accept(writer);
	data = buffer.GetString();
	jsonSize = static_cast<unsigned int>(buffer.GetSize());
	AZ_Printf("PLY", "Writing to conf file: %s", buffer.GetString());

	using namespace AZ::IO;

	try
	{
		HandleType fileHandle = InvalidHandle;
		FileIOBase *f = FileIOBase::GetInstance();
		if (f->Open("config/PLYConfig.json", OpenMode::ModeWrite | OpenMode::ModeBinary, fileHandle))
		{
			//Save data to the config file.
			f->Write(fileHandle, data, jsonSize);

			f->Close(fileHandle);
		}
		else
		{
			PLYLOG(PLYLog::PLY_ERROR, "Couldn't save PLY config file. Unable to open PLYConfig.json for saving.");
		}
	}
	catch (const std::exception &e)
	{
		PLYLOG(PLYLog::PLY_ERROR, "Couldn't save PLY config file. Error: " + AZStd::string(e.what()));
	}
}

template<class T>
void PLYConfigFileHandler::AddConfEntry(std::string group, std::string name, T set, std::vector<ConfEntry> &vec)
{
	AZ_Printf("PLY", "%s", typeid(T).name());

	CType thisType = CType::NONE;
	if (typeid(T) == typeid(int*))
	{
		thisType = CType::INT;
	}
	else if (typeid(T) == typeid(unsigned int*))
	{
		thisType = CType::UNSIGNED_INT;
	}
	else if (typeid(T) == typeid(AZStd::string*))
	{
		thisType = CType::STRING;
	}
	else if (typeid(T) == typeid(bool*))
	{
		thisType = CType::BOOL;
	}
	else if (typeid(T) == typeid(PLYLog::LogLevel *))
	{
		thisType = CType::LOGLEVEL;
	}
	else if (typeid(T) == typeid(PoolSettings::Priority *))
	{
		thisType = CType::PRIORITY;
	}
	else if (typeid(T) == typeid(PoolSettings::WaitMode *))
	{
		thisType = CType::WAITMODE;
	}
	else if (typeid(T) == typeid(DatabaseConnectionDetails::SSLMode *))
	{
		thisType = CType::SSLMODE;
	}
	else
	{
		//ERROR - Unknown type, so do not add this entry to the array.
		PLYLOG(PLYLog::PLY_ERROR, ("Invalid type " + AZStd::string(typeid(T).name()) + " sent to PLYConfigFileHandler AddConfEntry.").c_str());
		return;
	}

	vec.push_back(ConfEntry({ group, name, thisType, &set }));
}

void PLYConfigFileHandler::Load()
{

	std::unique_lock<std::mutex> lock(m_configMutex);

	//Buffer for file data.
	char *data;

	//Context block to limit using namespace scope.
	{
		using namespace AZ::IO;

		try
		{
			HandleType fileHandle = InvalidHandle;
			FileIOBase *f = FileIOBase::GetInstance();

			if (f->Exists("config/PLYConfig.json"))
			{
				if (f->Open("config/PLYConfig.json", OpenMode::ModeRead | OpenMode::ModeBinary, fileHandle))
				{
					//Get the file size in bytes.
					AZ::u64 fsize;
					f->Size(fileHandle, fsize);
					
					//Allocate memory for buffer.
					//Add extra character to end of char* length, as we will add a null terminator.
					data = new char[fsize + 1];
					
					//Read file data into the buffer.
					f->Read(fileHandle, data, fsize);

					//Null terminate char*. Remember index "fsize" will now refer to the last character in the char*,
					//as we added +1 to size above (index starts at 0).
					data[fsize] = '\0';

					f->Close(fileHandle);
				}
				else
				{
					PLYLOG(PLYLog::PLY_ERROR, "Couldn't load PLY config file PLYConfig.json.");
					return;
				}
			}
			else
			{
				//Config file doesn't exist yet.
				PLYLOG(PLYLog::PLY_WARNING, "Couldn't load PLY config file PLYConfig.json. This is normal the first time PLY is run.");

				//Abort loading. This will just keep internal defaults.
				return;
			}
		}
		catch (const std::exception &e)
		{
			PLYLOG(PLYLog::PLY_ERROR, "Couldn't load PLY config file. Error: " + AZStd::string(e.what()));
		}

	}

	if (data == nullptr)
	{
		PLYLOG(PLYLog::PLY_WARNING, "Couldn't load data from PLY config file PLYConfig.json.");
		return;
	}

	using namespace rapidjson;

	//AZ_Printf("PLY", "%s", data);

	Document d;

	try
	{
		d.Parse(data);
	}
	catch (const std::exception &e)
	{
		PLYLOG(PLYLog::PLY_ERROR, "Bad JSON data. Error: " + AZStd::string(e.what()));
	}

	Value *v;

	std::vector<ConfEntry> entries;
	AddConfEntry<>("DatabaseSettings", "host", &m_d.host, entries);
	AddConfEntry<>("DatabaseSettings", "port", &m_d.port, entries);
	AddConfEntry<>("DatabaseSettings", "sslMode", &m_d.sslMode, entries);
	AddConfEntry<>("DatabaseSettings", "database", &m_d.database, entries);
	AddConfEntry<>("DatabaseSettings", "username", &m_d.username, entries);
	AddConfEntry<>("DatabaseSettings", "password", &m_d.password, entries);
	AddConfEntry<>("DatabaseSettings", "connectTimeout", &m_d.connectTimeout, entries);
	AddConfEntry<>("DatabaseSettings", "reconnectWaitTime", &m_d.reconnectWaitTime, entries);

	AddConfEntry<>("QuerySettings", "queryTTL", &m_qs.queryTTL, entries);
	AddConfEntry<>("QuerySettings", "resultTTL", &m_qs.resultTTL, entries);
	AddConfEntry<>("QuerySettings", "useTransaction", &m_qs.useTransaction, entries);
	AddConfEntry<>("QuerySettings", "advertiseResult", &m_qs.advertiseResult, entries);

	AddConfEntry<>("PoolSettings", "managerPriority", &m_p.managerPriority, entries);
	AddConfEntry<>("PoolSettings", "workerPriority", &m_p.workerPriority, entries);
	AddConfEntry<>("PoolSettings", "waitMode", &m_p.waitMode, entries);
	AddConfEntry<>("PoolSettings", "minPoolSize", &m_p.minPoolSize, entries);
	AddConfEntry<>("PoolSettings", "maxPoolSize", &m_p.maxPoolSize, entries);

	AddConfEntry<>("Log", "logLevel", &m_logLevel, entries);

	for (auto &e : entries)
	{
		v = Pointer(("/" + e.group + "/" + e.name).c_str()).Get(d);
		
		if (v == nullptr)
		{
			PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/") 
				+ e.name.c_str() + AZStd::string(" value from config file. Entry not found."));
			continue;
		}

		if (e.type == CType::STRING)
		{
			if (!v->IsString())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. String type entry expected."));
				continue;
			}
			AZStd::string *pS = static_cast<AZStd::string *>(e.set);
			*pS = v->GetString();
		} 
		else if (e.type == CType::INT)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			int *pI = static_cast<int *>(e.set);
			*pI = v->GetInt();
		}
		else if (e.type == CType::UNSIGNED_INT)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			if (v->GetInt() < 0)
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer for this entry must not be negative."));
				continue;
			}
			unsigned int *pU = static_cast<unsigned int *>(e.set);
			*pU = v->GetInt();
		}
		else if (e.type == CType::BOOL)
		{
			if (!v->IsBool())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Boolean type entry expected."));
				continue;
			}
			bool *pB = static_cast<bool *>(e.set);
			*pB = v->GetBool();
		}
		else if (e.type == CType::LOGLEVEL)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			PLYLog::LogLevel *pL = static_cast<PLYLog::LogLevel *>(e.set);
			*pL = static_cast<PLYLog::LogLevel>(v->GetInt());
		}
		else if (e.type == CType::PRIORITY)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			PoolSettings::Priority *pP = static_cast<PoolSettings::Priority *>(e.set);
			*pP = static_cast<PoolSettings::Priority>(v->GetInt());
		}
		else if (e.type == CType::WAITMODE)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			PoolSettings::WaitMode *pW = static_cast<PoolSettings::WaitMode *>(e.set);
			*pW = static_cast<PoolSettings::WaitMode>(v->GetInt());
		}
		else if (e.type == CType::SSLMODE)
		{
			if (!v->IsInt())
			{
				PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
					+ e.name.c_str() + AZStd::string(" value from config file. Integer type entry expected."));
				continue;
			}
			DatabaseConnectionDetails::SSLMode *pS = static_cast<DatabaseConnectionDetails::SSLMode *>(e.set);
			*pS = static_cast<DatabaseConnectionDetails::SSLMode>(v->GetInt());
		}
		else
		{
			PLYLOG(PLYLog::PLY_ERROR, AZStd::string("Error reading /") + e.group.c_str() + AZStd::string("/")
				+ e.name.c_str() + AZStd::string(" Unknown type selected in ConfEntry CType."));
			continue;
		}
	}

	RegenConnectionString();

	//Clean up file buffer.
	delete[] data;

}

void PLYConfigFileHandler::SetConfigValues(const PLY::DatabaseConnectionDetails d, const PLY::QuerySettings qs,
	const PLY::PoolSettings p, const PLY::PLYLog::LogLevel l)
{
	std::unique_lock<std::mutex> lock(m_configMutex);

	//Log level.
	PLYLOG_SET_LEVEL(l);

	//Set new query settings.
	m_qs = qs;

	//Set new connection details.
	m_d = d;

	RegenConnectionString();

	//Set new pool settings.
	m_p = p;

	//Use min and max of pool values in case the user assigned them the wrong way around.
	m_p.minPoolSize = std::min(m_p.minPoolSize, m_p.maxPoolSize);
	m_p.maxPoolSize = std::max(m_p.minPoolSize, m_p.maxPoolSize);

	//Set sensible limits.
	if (m_p.minPoolSize < 1) m_p.minPoolSize = 1;
	if (m_p.maxPoolSize > 2048) m_p.maxPoolSize = 2048;

	Save();

	PLYLOG(PLYLog::PLY_DEBUG, "PLY config values set");
}

DatabaseConnectionDetails PLYConfigFileHandler::GetDatabaseConnectionDetails()
{
	std::unique_lock<std::mutex> lock(m_configMutex);
	return m_d;
}

QuerySettings PLYConfigFileHandler::GetQuerySettings()
{
	std::unique_lock<std::mutex> lock(m_configMutex);
	return m_qs;
}

PoolSettings PLYConfigFileHandler::GetPoolSettings()
{
	std::unique_lock<std::mutex> lock(m_configMutex);
	return m_p;
}

PLYLog::LogLevel PLYConfigFileHandler::GetLogLevel()
{
	std::unique_lock<std::mutex> lock(m_configMutex);
	return m_logLevel;
}

AZStd::string PLYConfigFileHandler::GetConnectionString()
{
	std::unique_lock<std::mutex> lock(m_configMutex);
	return m_connectionString;
}
*/