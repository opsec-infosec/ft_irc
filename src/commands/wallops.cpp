/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wallops.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 21:25:13 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:09:56 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
 ERR_UNKNOWNCOMMAND
*/

// @brief Wallops sends a message to all users with the +w mode
Wallops::Wallops( ) : ICommand( ) {

}

Wallops::~Wallops( ) {

}

void Wallops::cmdExecute( void ) {
    Clients* client;
    try { client = getClient(); }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( !client->isOperator() )
        return cmdError( ERR_NOPRIVILEGES );

    std::string trailing = getTrailing();
    if ( trailing.empty() )
        return  cmdError( ERR_NEEDMOREPARAMS );

    wallops( client->getNickName(), trailing );
}

// @brief sends a wallops message to all users, this will honor the +w mode flag
//      if the source is empty, the wallop will be sent to all users ignoring the +w mode flag and source will be the server
//      this is used in cases where the server sends out messages to clients such as a critical error or restart...
// @params[in] source source of who is sending the message, ie this client
// @params[in] comment Message to send
// @exceptsafe no-throw
void Wallops::wallops( const std::string& source, const std::string& comment ) {
    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch ( ICommandException& ex ) { return ; }

    std::vector<Clients *> clients;
    try { clients = rtd->getClientList(); }
    catch ( std::logic_error& ex ) { return ; }

    if ( source.empty() ) {
        sendReplyToList( clients, libft::to_string( SERVER ), "WALLOPS", comment );
        return ;
    }

    for ( std::vector<Clients *>::iterator it = clients.begin(); it != clients.end(); ++it ) {
        if ( (*it.base())->getMode() & 4 )
            sendReplyTo( (*it.base())->getNickName(), source, "WALLOPS", comment );
    }
}
