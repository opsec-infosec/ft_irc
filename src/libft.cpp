/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:11:37 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 02:17:27 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/libft.hpp"

namespace libft {
    // @brief Convert string to int
    // @param[in] val Converts const std::string val to int
    // @returns[out] int
    // @throws invalid_argument exception if val is not a int or val cannot fit in a int
    // @exceptsafe strong
    int stoi( const std::string& val ) {
        for ( std::string::const_iterator it = val.begin() ; it != val.end(); it++ ) {
            if ( !isdigit(*it) )
                throw std::invalid_argument( "Invalid int" );
        }

        long tmp = std::atol( val.c_str() );

        if ( tmp > std::numeric_limits<int>::max() || tmp < std::numeric_limits<int>::min() )
            throw std::invalid_argument( "Invalid int" );

        return std::atoi( val.c_str() );
    }

    // @brief Convert string to long
    // @param[in] val Converts const std::string to long
    // @returns[out] long
    // @throws invalid_argument exception if val is not a long or val cannot fit in a long
    // @exceptsafe strong
    long stol( const std::string& val ) {
        for ( std::string::const_iterator it = val.begin() ; it != val.end(); it++ ) {
            if ( !isdigit( *it ) )
                throw std::invalid_argument( "Invalid long int" );
        }

        long long tmp = std::atoll( val.c_str( ) );

        if ( tmp > std::numeric_limits<long>::max() || tmp < std::numeric_limits<long>::min() )
            throw std::invalid_argument( "Invalid long int" );

        return std::atol( val.c_str() );
    }

    // @brief Convert string from lower case to upper case
    // @params[in] val string to convert
    // @returns[out] std::string uppercase version of val
    std::string to_upper( const std::string& val ) {
        std::string ret;

        if ( val.empty() )
            return ret;

        for ( std::string::const_iterator it = val.begin(); it != val.end(); ++it ) {
            ret += std::toupper( static_cast<unsigned char>( *it ) );
        }

        return ret;
    }

    // @brief Splits String based on delimiter
    // @param[in] val of string to split
    // @param[in] delim char to split string on
    // @returns[out] std::queue<std::string>
    // @exceptsafe no-throw
    std::queue<std::string> splitStr( std::string& val, const std::string& delim ) {
        std::queue<std::string> ret;
        size_t pos = 0;

        while ( (pos = val.find( delim ) ) != std::string::npos ) {
            std::string word = val.substr( 0, pos );

            if ( word != "" )
                ret.push( val.substr( 0, pos ) );

            val.erase(0, pos + delim.length());
        }
        return ret;
    }

    // @brief Splits String based on Space and splits params off the end, irc specific
    // @param[in] val of string to split
    // @returns[out] std::vector<std::string>
    // @exceptsafe no-throw
    std::vector<std::string> splitSpace( const std::string& val ) {
        std::vector<std::string> words;
        std::string tmp;
        std::string trailing;

        if ( val.empty() )
            return words;

        size_t pos = val.find_first_of(':');

        if ( pos != std::string::npos )
            trailing = val.substr( pos );

        std::stringstream sst( val.substr( 0, pos ) );
        while ( sst>>tmp )
            words.push_back( tmp );

        if ( !trailing.empty() )
            words.push_back( trailing );

        return words;
    }

    // @brief Checks a string to see if it is isalnum (alpha numeric)
    // @params[in] val string value to check
    // @returns[out] boolen, true if alpha numeric
    // @exceptsafe no-throw
    bool isalnumStr( const std::string& val ) {
        for ( std::string::const_iterator it = val.begin(); it != val.end(); ++it ) {
            if ( !std::isalnum( *it ) )
                return false;
        }
        return true;
    }


    // @brief Returns the current time as a string
    // @returns[out] std::string Current Time as Www Mmm dd hh:mm:ss yyyy
    // @exceptsafe no-throw
    std::string getCurTimeString( void ) {
        std::time_t tmT;


        tmT = time( NULL );

        if ( tmT < 0 )
            return std::string();

        return std::ctime( &tmT );
    }

    // @brief Returns the current Unix Time since epoch as a string
    // @returns[out] std::string Current time in seconds from epoch
    // @exceptsafe no-throw;
    std::string getCurUnixTimeString( void ) {
        std::stringstream ss;
        std::time_t tmT;

        tmT = time( NULL );
        ss << tmT;

        return ss.str();
    }
}
