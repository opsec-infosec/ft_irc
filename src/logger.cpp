/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:10:48 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 15:48:13 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/logger.hpp"

// @brief Logger Constructor, creates a file with YYYYMMDD_HHMMSS.log upon instantiation
Logger::Logger( ) : m_enable( LOG ) {
    m_filename = getQueryTime( false );
    m_output.open( m_filename.c_str(), std::fstream::out | std::fstream::app );
}

// @brief Logger Destructor, closes log file one destruction
Logger::~Logger() {
    if ( m_output.is_open() ) {
        m_output.flush();
        m_output.close();
    }

    if ( !m_enable )
        std::cout.flush();
}

// @brief Logger Copy Constructor
// @param[in, out] obj Copies const Logger& obj
Logger::Logger( Logger& obj ) {
    *this = obj;
}

// @brief Logger Copy Operator
// @param[in, out] obj Copies const Logger& obj
// @returns[out] Logger&
Logger&  Logger::operator=( Logger &obj ) {
    if ( this != &obj )
        *this = obj;

    return *this;
}

// Public Methods

// @brief Saves msg to a file created with YYYYMMDD.log and adds a timestamp of HH:MM:SS and the msg
// @param[in] msg Message to save in the created file
// @returns[out] std::string Timestamped msg "HH:MM:SS \t <msg> \n"
// @exceptsafe no-throw
std::string Logger::save( const std::string& msg ) {
    std::stringstream ret;

    ret << getQueryTime( true ) << "\t" << msg;
    writeFile( ret.str() );

    if ( !m_enable ) {
        std::cout << ret.str() << std::endl;
        std::cout.flush();
    }

    return ret.str();
}

// @brief Save msg to a file created with YYYMMDD with the class object to identify instantiated class, adds a timestamp of HH:MM:SS
// @param[in] obj Class instantiated object
// @returns[out] std::string
// @exceptsafe no-throw
std::string Logger::save( const std::string&  msg, const void* obj ) {
    std::stringstream ret;

    ret << getQueryTime( true ) << "\t" << obj << "\t" << msg;
    writeFile( ret.str() );

    if ( !m_enable ) {
        std::cout << ret.str() << std::endl;
        std::cout.flush();
    }

    return ret.str();
}

// @brief Returns the log filename
// @returns[out] std::string log filename
// @exceptsafe no-throw
std::string Logger::getFilename( void ) const {
    return m_filename;
}

// Private Methods

// @brief Creates a date string
// @param[in] timestamp If true, returns HH:MM:SS otherwise returns YYYYMMDD.log
// @exceptsafe no-throw
std::string Logger::getQueryTime( const bool& timestamp ) {
    std::stringstream ss;

    struct {
        std::time_t t;
        tm* tmPtr;
        int curYear;
        int curMonth;
        int curDay;
        int curHour;
        int curMin;
        int curSec;
        } curTime;

    curTime.t = time( NULL );
    if ( curTime.t == -1 )
        return std::string();

    if ( GMT )
        curTime.tmPtr = gmtime( &curTime.t );
    else
        curTime.tmPtr = localtime( &curTime.t );

    curTime.curYear = curTime.tmPtr->tm_year + 1900;
    curTime.curMonth = curTime.tmPtr->tm_mon + 1;
    curTime.curDay = curTime.tmPtr->tm_mday;
    curTime.curHour = curTime.tmPtr->tm_hour;
    curTime.curMin = curTime.tmPtr->tm_min;
    curTime.curSec = curTime.tmPtr->tm_sec;

    if ( !timestamp ) {
        ss << std::setfill( '0' ) << curTime.curYear << std::setw( 2 ) << curTime.curMonth <<  std::setw( 2 ) << curTime.curDay \
            << ".log";
    }
    else {
        ss << std::setfill( '0' ) << std::setw( 2 ) << curTime.tmPtr->tm_hour << ":" << std::setw( 2 ) \
            << curTime.tmPtr->tm_min << ":" << std::setw( 2 ) << curTime.tmPtr->tm_sec;
    }

    return ss.str();
}

// @brief Writes std::string msg to file
// @params[in] msg std::string message to write
// @exceptsafe no-throw
void Logger::writeFile( std::string msg ) {
    if ( m_enable && !m_filename.empty() && m_output.is_open() ) {
        m_output << msg << std::endl;
        m_output.flush();
    }
}

void Logger::enable( bool enable ) {
    m_enable = enable;
}
