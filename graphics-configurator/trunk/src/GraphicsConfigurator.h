#pragma once
#pragma warning(push, 1)
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/AutoPtr.h>
#pragma warning(pop)
#include <map>
#include <vector>

//------------------------------------------------------
struct TBaseCommandParam
{
	TBaseCommandParam(const std::string& name)
		: Name(name)
	{
	}

	std::string Name;
};

//------------------------------------------------------
struct TCommandParam : public TBaseCommandParam
{
	TCommandParam(const std::string& name, const std::string& value)
		: TBaseCommandParam(name)
		, Value(value)
	{
	}

	std::string Value;
};

//------------------------------------------------------
struct TCommand
{
	std::string CommandName;
	std::vector<TCommandParam> Params;
};

class GraphicsConfigurator;
//------------------------------------------------------
struct TBaseCommand
{
	void RegisterCallback(GraphicsConfigurator* caller, void(GraphicsConfigurator::*callback)(TCommand&))
	{
		m_pCaller = caller;
		m_pCallback = callback;	
	}
	
	void Process(TCommand& cmd)
	{
		(m_pCaller->*m_pCallback)(cmd);
	}

	std::vector<TBaseCommandParam> Params;

private:
	void(GraphicsConfigurator::*m_pCallback)(TCommand&);
	GraphicsConfigurator* m_pCaller;
};

//------------------------------------------------------
struct TBaseConfigField
{
	void RegisterCheckCallback(GraphicsConfigurator* caller, bool(GraphicsConfigurator::*callback)(const std::string&, const std::string&))
	{
		m_pCaller = caller;
		m_pCheckCallback = callback;	
	}
	
	void RegisterSetCallback(GraphicsConfigurator* caller, void(GraphicsConfigurator::*callback)(Poco::AutoPtr<Poco::Util::XMLConfiguration>, const std::string&, const std::string&))
	{
		m_pCaller = caller;
		m_pSetCallback = callback;	
	}

	bool Check(const std::string& name, const std::string& value)
	{
		return (m_pCaller->*m_pCheckCallback)(name, value);
	}

	void Set(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value)
	{
		return (m_pCaller->*m_pSetCallback)(config, name, value);
	}

	std::vector<std::string> Values;
	std::string DefaultValue;

private:
	bool(GraphicsConfigurator::*m_pCheckCallback)(const std::string&, const std::string&);
	void(GraphicsConfigurator::*m_pSetCallback)(Poco::AutoPtr<Poco::Util::XMLConfiguration>, const std::string&, const std::string&);
	GraphicsConfigurator* m_pCaller;
};

//------------------------------------------------------
typedef std::map<std::string, TBaseCommand> TBaseCommands;
typedef std::map<std::string, TBaseConfigField> TBaseConfigFields;

//------------------------------------------------------
typedef enum
{
	OFILE_XML,
	OFILE_BINARY
} OutFileType;

typedef enum
{
	BQUALITY_LOW,
	BQUALITY_MEDIUM,
	BQUALITY_HIGHT
} BaseQuality;

typedef enum
{
	GQUALITY_LOW,
	GQUALITY_NORMAL,
	GQUALITY_HIGHT,
	GQUALITY_CUSTOM
} GraphicsQuality;

typedef enum
{
	RESOLUTION_640x480,
	RESOLUTION_1024x768,
	RESOLUTION_1920x1080
} Resulution;

typedef enum
{
	ANTIALIASING_MULTISAMPLE_NONE,
	ANTIALIASING_MULTISAMPLE_2,
	ANTIALIASING_MULTISAMPLE_4
} Antialiasing;

//------------------------------------------------------

/*
	Класс GraphicsConfigurator
*/
class GraphicsConfigurator
{
public:
	GraphicsConfigurator();
	GraphicsConfigurator(Poco::AutoPtr<Poco::Util::XMLConfiguration> config);
	void Save();
	bool IsTerminated();
	void Run();

	/*
	**	Функции обработки команд
	*/
	void ProcessChangeCommand(TCommand& cmd);
	void ProcessGetCommand(TCommand& cmd);
	void ProcessPrintCommand(TCommand& cmd);
	void ProcessFormatCommand(TCommand& cmd);
	void ProcessSaveCommand(TCommand& cmd);
	void ProcessExitCommand(TCommand& cmd);

	/*
	**	Колбэки проверки значений полей
	*/
	bool CheckFieldSet(const std::string& name, const std::string& value);
	bool CheckFieldBool(const std::string& name, const std::string& value);
	bool CheckViewDistance(const std::string& name, const std::string& value);
	
	/*
	**	Колбэки установки значений полей
	*/
	void SetOneOfGraphicsQuality(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value);
	void SetGraphicsQuality(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value);
	void SetFieldSimple(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value);

private:
	/*
	** RegisterCommands - регистрация команд
	*/
	void RegisterCommands();

	/*
	** RegisterConfigFields - регистрация полей конфигурации
	*/
	void RegisterConfigFields();

	/*
	** ReceiveCommand - чтение команды из стандартного потока ввода
	*/
	bool ReceiveCommand(TCommand& outCmd);


	Poco::AutoPtr<Poco::Util::XMLConfiguration> m_pConfig;

	/*
	** Commands - список поддерживаемых команд
	*/
	TBaseCommands m_commands;

	/*
	** TBaseConfigFields - список поддерживаемых полей
	*/
	TBaseConfigFields m_configFields;

	bool m_terminated;
	OutFileType m_outFileType;

	void SaveXML(const std::string& fileName);
	void SaveBinary(const std::string& fileName);
};