// Logger.h created on 2018-03-01, part of XCI toolkit

#include <xci/core/log.h>
#include <xci/core/Term.h>
#include <xci/core/sys.h>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cassert>
#include <unistd.h>
#include "log.h"


namespace xci::core {


static const char* level_format[] = {
        "{:19} {cyan}{}{normal}  {bold}DEBUG{normal}  {white}{}{normal}\n",
        "{:19} {cyan}{}{normal}  {bold}INFO {normal}  {bold}{white}{}{normal}\n",
        "{:19} {cyan}{}{normal}  {bold}WARN {normal}  {bold}{yellow}{}{normal}\n",
        "{:19} {cyan}{}{normal}  {bold}ERROR{normal}  {bold}{red}{}{normal}\n",
};


Logger& Logger::default_instance(Logger::Level initial_level)
{
    static Logger logger(initial_level);
    return logger;
}


Logger::Logger(Level level) : m_level(level)
{
    if (m_level <= Level::Info) {
        Term& t = Term::stderr_instance();
        auto msg = t.format("{underline}   Date      Time    TID   Level  Message   {normal}\n");
        ::write(STDERR_FILENO, msg.data(), msg.size());
    }
}


Logger::~Logger()
{
    if (m_level <= Level::Info) {
        Term& t = Term::stderr_instance();
        auto msg = t.format("{overline}                 End of Log                 {normal}\n");
        ::write(STDERR_FILENO, msg.data(), msg.size());
    }
}


static inline std::string format_current_time()
{
    time_t now = std::time(nullptr);
    std::string ts_buf(20, '\0');
    size_t ts_res = std::strftime(&ts_buf[0], ts_buf.size(), "%F %T", std::localtime(&now));
    assert(ts_res > 0 && ts_res < ts_buf.size());
    ts_buf.resize(ts_res);
    return ts_buf;
}


void Logger::default_handler(Logger::Level lvl, const std::string& msg)
{
    Term& t = Term::stderr_instance();
    auto lvl_num = static_cast<int>(lvl);
    auto formatted_msg = t.format(level_format[lvl_num], format_current_time(), get_thread_id(), msg);
    ::write(STDERR_FILENO, formatted_msg.data(), formatted_msg.size());
}


void Logger::log(Logger::Level lvl, const std::string& msg)
{
    if (lvl < m_level)
        return;

    m_handler(lvl, msg);
}


} // namespace xci::core
