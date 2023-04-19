/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   restart.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/25 23:56:19 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/11 10:59:22 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NOPRIVILEGES (481)
*/

// @brief Restart simulates a restarting of the server, closing all client
// connections.
Restart::Restart( ) : ICommand( ) {

}

Restart::~Restart( ) {

}

void Restart::cmdExecute( void ) {
    Clients* client;
    try { client = getClient(); }
    catch ( std::runtime_error& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( !client->isOperator() )
        return cmdError( ERR_NOPRIVILEGES );

    restart();
}

void Restart::restart( void ) {
    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch( ICommandException& ex) { return ; }

    ICommand* icmd = new Wallops();
    icmd->loadRunTimeData( rtd );

    std::vector<Clients *> clients;
    try { clients = rtd->getClientList(); }
    catch ( std::logic_error& ex ) {
        delete icmd;
        return ;
    }

    ((Wallops *)icmd)->wallops( "", "ATTENTION RESTARTING SERVER " + libft::to_string( SERVER) );

    for ( std::vector<Clients *>::iterator it = clients.begin(); it != clients.end(); ++it ) {
        close( (*it.base())->getFD() );
    }

    delete icmd;
}
