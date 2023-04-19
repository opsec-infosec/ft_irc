/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:10:59 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 15:25:19 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
# define LOGGER_HPP

# ifndef LOG
#  define LOG true
# endif
# ifndef GMT
#  define GMT true
# endif

# include <ctime>
# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include <iomanip>
# include <typeinfo>
# include <exception>


class Logger {
    public:
        // Constructor Destructor
        Logger( );
        ~Logger( );

        // Copy Constructor Copy Operator
        Logger( Logger& obj );
        Logger& operator=( Logger& obj );

        // Public Methods
        std::string save( const std::string& msg );
        std::string save( const std::string& msg, const void* obj );
        void enable( bool enable );

        // Getters
        std::string getFilename( void ) const;

    private:
        std::string m_filename;
        std::ofstream m_output;
        bool m_enable;

        std::string getQueryTime( const bool& timestamp );
        void writeFile( std::string msg );
};

#endif
