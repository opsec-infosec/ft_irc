/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:10:12 by dfurneau          #+#    #+#             */
/*   Updated: 2023/03/26 21:11:04 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/server.hpp"
#include "../includes/logger.hpp"
#include "../includes/libft.hpp"

#include <cstdlib>
#include <iostream>

// @brief Prints out usage of program
// @param[in] prg Name of Program
// @returns[out] int Returns EXIT_FAILURE
int usage( const std::string prg ) {
    std::cout << "USAGE: ";
    std::cout << prg.substr( prg.find_last_of( '/' ) + 1 ) << " <port> <password>" << std::endl;
    std::cout << "  <port> = Port number to listen on (0 < 65535)" << std::endl;
    std::cout << "  <password> = Password to connect to irc server" << std::endl;
    return EXIT_FAILURE;
}

int main ( int ac, char** av ) {
    int port;
    std::string password;

    if ( ac != 3 )
        return usage( av[0] );

    try {
        port = libft::stoi( av[1] );
        if ( port <= 0 || port > 65535 )
            return usage( av[0] );
    }
    catch ( std::invalid_argument& ex ) {
        return usage( av[0] );
    }

    password = av[2];

    irc::Server* ircserv = new irc::Server( "0.0.0.0", port, password );
    try {
        ircserv->init();
        ircserv->start();
    }
    catch ( std::runtime_error& ex ) {
        std::cout << ex.what() << std::endl;
        delete ircserv;
        return EXIT_FAILURE;
    }
    catch ( std::logic_error& ex ) {
        std::cout << ex.what() << std::endl;
        delete ircserv;
        return EXIT_FAILURE;
    }
    catch ( ... ) {
        std::cout << "Fatal Error - Uncaught Exception" << std::endl;
        delete ircserv;
        return EXIT_FAILURE;
    }

    delete ircserv;
    return EXIT_SUCCESS;
}
