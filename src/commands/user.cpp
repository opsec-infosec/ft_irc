/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 22:21:14 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/02 17:30:22 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_ALREADYREGISTERED (462)
*/

// @brief User Command for registering with the irc server
User::User( ) : ICommand( ) {

}

User::~User( ) {

}

void User::cmdExecute( void ) {
    Clients* client;
    std::vector<std::string> params;
    std::string trailing = getTrailing();

    try {
        client = getClient();
        params = getParams();
    }
    catch( ICommandException& ex ) { return ; }

    if ( client->isRegistered() )
        return cmdError( ERR_ALREADYREGISTERED );

    if ( !client->isAuthenticated() ) {
        sendReply ( libft::to_string( SERVER ) + ": ERROR :Password Incorrect" + "\r\n" );
        close( getClient()->getFD() );
        return ;
    }

    if ( params.size() < 3 || trailing.empty() )
        return cmdError( ERR_NEEDMOREPARAMS );

    client->setUserName( params[0] );
    client->setHostName( params[1] );
    client->setConnHostName( params[2] );
    client->setRealName( trailing );
}
