#pragma warning(push, 1)
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/XMLConfiguration.h>
#pragma warning(pop)

#include "GraphicsConfigurator.h"

using Poco::AutoPtr;
using Poco::Util::Application;
using Poco::Util::XMLConfiguration;

/*
	Класс SocialQuantumTest - точка входа в приложение
*/
class SocialQuantumTest: public Application
{
public:
	SocialQuantumTest()
		: m_configPath("config.xml")
	{
		
	}

protected:
	void initialize(Application& self)
	{
		Application::initialize(self);
		try
		{
			m_pConfig = AutoPtr<XMLConfiguration>(new XMLConfiguration(m_configPath));
		}
		catch(Poco::Exception& ex)
		{
			std::cerr << "Configuration '" << m_configPath << "' can't be loaded (corrupted or not found)" << std::endl;
		}
	}

	void uninnitialize()
	{
		
	}

	int main(const std::vector<std::string>& args)
	{
		try
		{
			std::cout << "Application started" << std::endl << std::endl;

			GraphicsConfigurator graphicsConfigurator(m_pConfig);
			graphicsConfigurator.Run();

			std::cout << "Application was terminated" << std::endl;

			return 0;
		}
		catch (const std::exception &exc)
		{
			std::cerr << "Fatal error: " << exc.what() << std::endl;			
		}

		return 0;
	}

	AutoPtr<XMLConfiguration> m_pConfig;

private:
	std::string m_configPath;
	
};

POCO_APP_MAIN(SocialQuantumTest)