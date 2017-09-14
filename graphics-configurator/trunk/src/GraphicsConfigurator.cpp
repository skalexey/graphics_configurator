#pragma warning(push, 1)
#include <Poco/Util/Application.h>
#include <Poco/NumberParser.h>
#pragma warning(pop)

#include <fstream>
#include <iostream>

#include "GraphicsConfigurator.h"

using Poco::AutoPtr;
using Poco::Util::Application;
using Poco::Util::XMLConfiguration;

GraphicsConfigurator::GraphicsConfigurator()
{
}

GraphicsConfigurator::GraphicsConfigurator(AutoPtr<XMLConfiguration> config)
	: m_terminated(false)
	, m_pConfig(config)
	, m_outFileType(OFILE_XML)
{
	RegisterCommands();
	RegisterConfigFields();
}

void GraphicsConfigurator::RegisterCommands()
{
	TBaseCommand cmd_change;
	TBaseCommand cmd_get;
	TBaseCommand cmd_print;
	TBaseCommand cmd_format;
	TBaseCommand cmd_save;
	TBaseCommand cmd_exit;

	cmd_change.Params.push_back(TBaseCommandParam("SettingName"));
	cmd_change.Params.push_back(TBaseCommandParam("SettingValue"));
	cmd_change.RegisterCallback(this, &GraphicsConfigurator::ProcessChangeCommand);

	cmd_get.Params.push_back(TBaseCommandParam("SettingName"));
	cmd_get.RegisterCallback(this, &GraphicsConfigurator::ProcessGetCommand);
	
	cmd_print.RegisterCallback(this, &GraphicsConfigurator::ProcessPrintCommand);
	
	cmd_format.Params.push_back(TBaseCommandParam("FormatType"));
	cmd_format.RegisterCallback(this, &GraphicsConfigurator::ProcessFormatCommand);

	cmd_save.Params.push_back(TBaseCommandParam("FileName"));
	cmd_save.RegisterCallback(this, &GraphicsConfigurator::ProcessSaveCommand);

	cmd_exit.RegisterCallback(this, &GraphicsConfigurator::ProcessExitCommand);

	m_commands["change"] = cmd_change;
	m_commands["get"] = cmd_get;
	m_commands["print"] = cmd_print;
	m_commands["format"] = cmd_format;
	m_commands["save"] = cmd_save;
	m_commands["exit"] = cmd_exit;
}

void GraphicsConfigurator::RegisterConfigFields()
{
	TBaseConfigField resolution;
	TBaseConfigField fullScreenMode;
	TBaseConfigField antialiasing;
	TBaseConfigField viewDistance;

	TBaseConfigField textureQuality;
	TBaseConfigField dynamicLighting;
	TBaseConfigField shadowsQuality;
	TBaseConfigField graphicsQuality;

	resolution.Values.push_back("640*480");
	resolution.Values.push_back("1024*768");
	resolution.Values.push_back("1920*1080");
	resolution.DefaultValue = "640*480";

	fullScreenMode.DefaultValue = "true";

	antialiasing.Values.push_back("MULTISAMPLE_NONE");
	antialiasing.Values.push_back("MULTISAMPLE_2");
	antialiasing.Values.push_back("MULTISAMPLE_4");
	antialiasing.DefaultValue = "MULTISAMPLE_NONE";

	viewDistance.DefaultValue = "500";

	textureQuality.Values.push_back("Low");
	textureQuality.Values.push_back("Medium");
	textureQuality.Values.push_back("Hight");

	shadowsQuality.Values.push_back("Low");
	shadowsQuality.Values.push_back("Medium");
	shadowsQuality.Values.push_back("Hight");

	graphicsQuality.Values.push_back("Low");
	graphicsQuality.Values.push_back("Normal");
	graphicsQuality.Values.push_back("Hight");
	graphicsQuality.Values.push_back("Costom");

	graphicsQuality.DefaultValue = "Normal";

	resolution.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldSet);
	fullScreenMode.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldBool);
	antialiasing.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldSet);
	viewDistance.RegisterCheckCallback(this, &GraphicsConfigurator::CheckViewDistance);
	textureQuality.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldSet);
	dynamicLighting.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldBool);
	shadowsQuality.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldSet);
	graphicsQuality.RegisterCheckCallback(this, &GraphicsConfigurator::CheckFieldSet);

	resolution.RegisterSetCallback(this, &GraphicsConfigurator::SetFieldSimple);
	fullScreenMode.RegisterSetCallback(this, &GraphicsConfigurator::SetFieldSimple);
	antialiasing.RegisterSetCallback(this, &GraphicsConfigurator::SetFieldSimple);
	viewDistance.RegisterSetCallback(this, &GraphicsConfigurator::SetFieldSimple);
	textureQuality.RegisterSetCallback(this, &GraphicsConfigurator::SetOneOfGraphicsQuality);
	dynamicLighting.RegisterSetCallback(this, &GraphicsConfigurator::SetOneOfGraphicsQuality);
	shadowsQuality.RegisterSetCallback(this, &GraphicsConfigurator::SetOneOfGraphicsQuality);
	graphicsQuality.RegisterSetCallback(this, &GraphicsConfigurator::SetGraphicsQuality);

	m_configFields["resolution"] = resolution;
	m_configFields["full-screen_mode"] = fullScreenMode;
	m_configFields["antialiasing"] = antialiasing;
	m_configFields["view_distance"] = viewDistance;

	m_configFields["texture_quality"] = textureQuality;
	m_configFields["dynamic_lighting"] = dynamicLighting;
	m_configFields["shadows_quality"] = shadowsQuality;
	m_configFields["graphics_quality"] = graphicsQuality;
}

void GraphicsConfigurator::Save()
{

}

bool GraphicsConfigurator::IsTerminated()
{
	return m_terminated;
}

bool GraphicsConfigurator::ReceiveCommand(TCommand& outCmd)
{
	std::string cmdName;
	std::cout << " > ";
	std::cin >> cmdName;
	outCmd.CommandName = cmdName;
	TBaseCommands::iterator baseCommand = m_commands.find(cmdName);

	if(baseCommand == m_commands.end())
		return false;

	for (unsigned int i = 0; i  < (*baseCommand).second.Params.size(); i++)
	{
		std::string param;
		std::cin >> param;
		outCmd.Params.push_back(TCommandParam((*baseCommand).second.Params.at(0).Name, param));
	}

	(*baseCommand).second.Process(outCmd);
	std::cout << std::endl;

	return true;
}

void GraphicsConfigurator::Run()
{
	while(!m_terminated)
	{
		TCommand cmd;
		if(!ReceiveCommand(cmd))
			std::cout << std::endl << "Unknown command '" << cmd.CommandName << "'" << std::endl;

		std::cout << std::endl;
	}
}

void GraphicsConfigurator::ProcessChangeCommand(TCommand& cmd)
{
	std::string fieldName = cmd.Params.at(0).Value;
	std::string fieldValue = cmd.Params.at(1).Value;

	if(m_configFields.find(fieldName) == m_configFields.end())
	{
		std::cout << "Unknown field '" << fieldName << "'" << std::endl << std::endl;
		return;
	}

	if(m_configFields[fieldName].Check(fieldName, fieldValue))
	{
		m_configFields[fieldName].Set(m_pConfig, fieldName, fieldValue);
	}
	else
	{
		std::cout << "Invalid value '" << fieldValue << "' of field '" << fieldName << "'" << std::endl << std::endl;
	}
}

void GraphicsConfigurator::ProcessGetCommand(TCommand& cmd)
{
	std::string fieldName = cmd.Params.at(0).Value;

	if(m_pConfig->hasOption(fieldName))
		std::cout << fieldName << ": " << m_pConfig->getString(fieldName) << std::endl;
	else
		std::cout << "Field '" << fieldName << "' is undefined" << std::endl;
}

void GraphicsConfigurator::ProcessPrintCommand(TCommand& cmd)
{
	std::cout << std::endl;
	for(TBaseConfigFields::iterator it = m_configFields.begin(); it != m_configFields.end(); it++)
	{
		std::string fieldName = (*it).first;
		if(m_pConfig->hasOption(fieldName))
			std::cout << "\t" << fieldName << ":\t" << m_pConfig->getString(fieldName) << std::endl;
	}

	std::cout << std::endl;
}

void GraphicsConfigurator::ProcessFormatCommand(TCommand& cmd)
{
	std::string outFileType = cmd.Params.at(0).Value;
	if(outFileType == "xml")
	{
		m_outFileType = OFILE_XML;
		std::cout << "Output file type setted to XML" << std::endl;
	}
	else if(outFileType == "bin")
	{
		m_outFileType = OFILE_BINARY;
		std::cout << "Output file type setted to BINARY" << std::endl;
	}
	else
	{
		std::cout << "Unknown file type '" << outFileType << "'" << std::endl;
	}
}

void GraphicsConfigurator::ProcessSaveCommand(TCommand& cmd)
{
	switch(m_outFileType)
	{
	case OFILE_XML:
		SaveXML(cmd.Params.at(0).Value);
		break;

	case OFILE_BINARY:
		SaveBinary(cmd.Params.at(0).Value);
		break;
	}
}

void GraphicsConfigurator::ProcessExitCommand(TCommand& cmd)
{
	m_terminated = true;
}

bool GraphicsConfigurator::CheckFieldSet(const std::string& name, const std::string& value)
{
	TBaseConfigField& baseConfigField = m_configFields[name];
	for(std::vector<std::string>::iterator it = baseConfigField.Values.begin(); it != baseConfigField.Values.end(); it++)
	{
		if(value == *it)
			return true;
	}
	
	return false;
}

bool GraphicsConfigurator::CheckFieldBool(const std::string& name, const std::string& value)
{
	return (value == "true" || value == "false") ? true : false;
}


bool GraphicsConfigurator::CheckViewDistance(const std::string& name, const std::string& value)
{
	int distance;
	try
	{
		distance = Poco::NumberParser::parse(value);
	}
	catch(Poco::SyntaxException&)
	{
		std::cout << "Error while parsing a value '" << value << "' of a field '" << name << "'" << std::endl;
		return false;
	}

	return (distance >= 100 && distance <= 1000) ? true : false;
}



void GraphicsConfigurator::SetFieldSimple(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value)
{
	m_pConfig->setString(name, value);
}

void GraphicsConfigurator::SetGraphicsQuality(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value)
{
	m_pConfig->setString(name, value);

	if(value == "Low")
	{
		SetFieldSimple(config, "texture_quality", "Low");
		SetFieldSimple(config, "dynamic_lighting", "false");
		SetFieldSimple(config, "shadows_quality", "Low");
	}
	else if(value == "Normal")
	{
		SetFieldSimple(config, "texture_quality", "Medium");
		SetFieldSimple(config, "dynamic_lighting", "false");
		SetFieldSimple(config, "shadows_quality", "Medium");
	}
	else if(value == "Hight")
	{
		SetFieldSimple(config, "texture_quality", "Hight");
		SetFieldSimple(config, "dynamic_lighting", "true");
		SetFieldSimple(config, "shadows_quality", "Hight");
	}
	else if(value == "Custom")
	{
		
	}
}

void GraphicsConfigurator::SetOneOfGraphicsQuality(Poco::AutoPtr<Poco::Util::XMLConfiguration> config, const std::string& name, const std::string& value)
{
	SetFieldSimple(config, "graphics_quality", "Custom");
}

void GraphicsConfigurator::SaveXML(const std::string& fileName)
{
	m_pConfig->save(fileName);
}

void GraphicsConfigurator::SaveBinary(const std::string& fileName)
{
	std::ofstream outputFileStream(fileName, std::ios::binary);
	for(TBaseConfigFields::iterator it = m_configFields.begin(); it != m_configFields.end(); it++)
	{
		std::string fieldName = (*it).first;
		TBaseConfigField field = (*it).second;
		std::string fieldValue = m_pConfig->getString(fieldName);
		unsigned int binFieldValue(0);
		if(fieldName == "resolution")
		{
			if(fieldValue == "640*480")
				binFieldValue = RESOLUTION_640x480;
			else if(fieldValue == "1024*768")
				binFieldValue = RESOLUTION_1024x768;
			else if(fieldValue == "1920*1080")
				binFieldValue = RESOLUTION_1920x1080;
		}
		else if(fieldName == "full-screen_mode" || fieldName == "dynamic_lighting")
		{
			if(fieldValue == "true")
				binFieldValue = 1;
			else if(fieldValue == "false")
				binFieldValue = 0;
		}
		else if(fieldName == "antialiasing")
		{
			if(fieldValue == "MULTISAMPLE_NONE")
				binFieldValue = ANTIALIASING_MULTISAMPLE_NONE;
			else if(fieldValue == "MULTISAMPLE_2")
				binFieldValue = ANTIALIASING_MULTISAMPLE_2;
			else if(fieldValue == "MULTISAMPLE_4")
				binFieldValue = ANTIALIASING_MULTISAMPLE_4;
		}
		else if(fieldName == "view_distance")
		{
			binFieldValue = Poco::NumberParser::parse(fieldValue);
		}
		else if(fieldName == "texture_quality" || fieldName == "shadows_quality")
		{
			if(fieldValue == "Low")
				binFieldValue = BQUALITY_LOW;
			else if(fieldValue == "Medium")
				binFieldValue = BQUALITY_MEDIUM;
			else if(fieldValue == "Hight")
				binFieldValue = BQUALITY_HIGHT;
		}
		else if(fieldName == "graphics_quality")
		{
			if(fieldValue == "Low")
				binFieldValue = GQUALITY_LOW;
			else if(fieldValue == "Normal")
				binFieldValue = GQUALITY_NORMAL;
			else if(fieldValue == "Hight")
				binFieldValue = GQUALITY_HIGHT;
			else if(fieldValue == "Custom")
				binFieldValue = GQUALITY_CUSTOM;
		}

		outputFileStream.write((char*)&fieldValue, sizeof(unsigned int));
	}

	outputFileStream.close();
}