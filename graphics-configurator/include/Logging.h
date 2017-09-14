#pragma once
#pragma warning(push, 1)
#include <string>
#include <sstream>
#include <iomanip>
#include <Poco/Logger.h>
#include <vector>
#pragma warning(pop)

//using mpw::loging::StreamLogger;

namespace mpw
{
	namespace loging
	{

		enum logLevel  { 
			native =		0,
			fatal =			Poco::Message::PRIO_FATAL, 
			critical =		Poco::Message::PRIO_CRITICAL, 
			error =			Poco::Message::PRIO_ERROR, 
			warning =		Poco::Message::PRIO_WARNING, 
			notice =		Poco::Message::PRIO_NOTICE, 
			information =	Poco::Message::PRIO_INFORMATION, 
			debug =			Poco::Message::PRIO_DEBUG, 
			trace  =		Poco::Message::PRIO_TRACE
		};

		class StreamLogger
		{
			std::string			m_sLogger;
			logLevel			m_sLoggerLevel;
			std::stringstream	m_ssConverter;
		public:
			StreamLogger(const std::string &logger, const logLevel& level = native)
				: m_sLogger(logger), m_sLoggerLevel(level)
			{
			}

			template <typename T>
			StreamLogger& operator << (const T &info)
			{
				m_ssConverter << info;
				return *this;
			}

			template <>
			StreamLogger& operator<< < std::vector<unsigned char> > (const std::vector<unsigned char>& arr)
			{
				m_ssConverter.write((const char*)arr.data(), arr.size());
				return *this;
			}

			StreamLogger& writeHex (const std::vector<unsigned char>& arr)
			{
				size_t pos = 0;
				for (std::vector<unsigned char>::const_iterator it = arr.begin(); it != arr.end(); ++it)
				{
					m_ssConverter << std::hex << std::setw(2) << std::setfill('0') << std::fixed << std::noshowbase  << std::uppercase << (unsigned short)(*it) << " ";
					if (++pos % 16 == 0) 
						m_ssConverter << std::endl;
				}
				
				return *this;
			}

			~StreamLogger()
			{
				Poco::Logger& ourlogger = Poco::Logger::get(m_sLogger);
				switch (m_sLoggerLevel)
				{
				case native:		ourlogger.log(m_ssConverter.str());			break;
				case fatal:			ourlogger.fatal(m_ssConverter.str());		break;
				case critical:		ourlogger.critical(m_ssConverter.str());	break;
				case error:			ourlogger.error(m_ssConverter.str());		break;
				case warning:		ourlogger.warning(m_ssConverter.str());		break;
				case notice:		ourlogger.notice(m_ssConverter.str());		break;
				case information:	ourlogger.information(m_ssConverter.str()); break;
				case debug:			ourlogger.debug(m_ssConverter.str());		break;
				case trace:			ourlogger.trace(m_ssConverter.str());		break;
				}
			}

			

		};
	}
}

#define DEFLOG(name,logger)			mpw::loging::StreamLogger name(logger,mpw::loging::native)
#define DEFLOG_FATAL(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::fatal)
#define DEFLOG_CRIT(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::critical)
#define DEFLOG_ERROR(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::error)
#define DEFLOG_WARN(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::warning)
#define DEFLOG_NOTE(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::notice)
#define DEFLOG_INFO(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::information)
#define DEFLOG_DEBUG(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::debug)
#define DEFLOG_TRACE(name,logger)	mpw::loging::StreamLogger name(logger,mpw::loging::trace)

#define LOGGER(logger)				mpw::loging::StreamLogger(logger,mpw::loging::native)
#define LOGGER_FATAL(logger)		mpw::loging::StreamLogger(logger,mpw::loging::fatal)
#define LOGGER_CRIT(logger)			mpw::loging::StreamLogger(logger,mpw::loging::critical)
#define LOGGER_ERROR(logger)		mpw::loging::StreamLogger(logger,mpw::loging::error) << __FUNCTION__ << ", line: " << __LINE__ << " : "
#define LOGGER_WARN(logger)			mpw::loging::StreamLogger(logger,mpw::loging::warning)
#define LOGGER_NOTE(logger)			mpw::loging::StreamLogger(logger,mpw::loging::notice)
#define LOGGER_INFO(logger)			mpw::loging::StreamLogger(logger,mpw::loging::information)
#define LOGGER_DEBUG(logger)		mpw::loging::StreamLogger(logger,mpw::loging::debug) << __FUNCTION__ << ", line: " << __LINE__ << " : "
#define LOGGER_TRACE(logger)		mpw::loging::StreamLogger(logger,mpw::loging::trace)

#define LOGS(logger,text)			LOGGER(logger) << text
#define LOGS_FATAL(logger,text)		LOGGER_FATAL(logger) << text
#define LOGS_CRIT(logger,text)		LOGGER_CRIT(logger) << text
#define LOGS_ERROR(logger,text)		LOGGER_ERROR(logger) << text
#define LOGS_WARN(logger,text)		LOGGER_WARN(logger) << text
#define LOGS_NOTE(logger,text)		LOGGER_NOTE(logger) << text
#define LOGS_INFO(logger,text)		LOGGER_INFO(logger) << text
#define LOGS_DEBUG(logger,text)		LOGGER_DEBUG(logger) << text
#define LOGS_TRACE(logger,text)		LOGGER_TRACE(logger) << text

#define LOG(text)					LOGS(g_loggerName, text)
#define LOG_FATAL(text)				LOGS_FATAL(g_loggerName, text)
#define LOG_CRIT(text)				LOGS_CRIT(g_loggerName, text)
#define LOG_ERROR(text)				LOGS_ERROR(g_loggerName, text)
#define LOG_WARN(text)				LOGS_WARN(g_loggerName, text)
#define LOG_NOTE(text)				LOGS_NOTE(g_loggerName, text)
#define LOG_INFO(text)				LOGS_INFO(g_loggerName, text)
#define LOG_DEBUG(text)				LOGS_DEBUG(g_loggerName, text)
#define LOG_TRACE(text)				LOGS_TRACE(g_loggerName, text)