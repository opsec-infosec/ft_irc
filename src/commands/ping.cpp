/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/23 21:14:12 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/01 15:14:42 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_NOORIGIN (409)
*/

// @brief Ping empty class non searcheable, returned if command not found
//  will basically do nothing and send the proper error message to client
Ping::Ping( ) : ICommand( ) {

}

Ping::~Ping( ) {

}

void Ping::cmdExecute( void ) {
    std::vector<std::string> params;

    try { params = getParams(); }
    catch( ICommandException& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }

    std::string token = getTrailing();
    std::string server = libft::to_string( SERVER );

    if ( token.empty() )
        sendReply(  server, "PONG", server, params.front() );
    else
        sendReply( server, "PONG", token );
}
