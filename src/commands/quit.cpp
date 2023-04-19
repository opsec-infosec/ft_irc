/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 19:43:02 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:07:33 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

// TODO Add notification to other clients that are in their channel

// @brief Quit command
Quit::Quit( ) : ICommand( ) {

}

Quit::~Quit( ) {

}

void Quit::cmdExecute( void ) {
    std::string trailing = getTrailing();

    Clients* client;
    try { client = getClient(); }
    catch ( ICommandException& ex ) { return ; }

    sendReply( client->getNickName(), getCmd(), " :QUIT: " + trailing  );
    sendReply( "ERROR :Closing Link: " + client->getNickName() + " (Quit: " + trailing + ")\r\n" );

    client->markForDeletion( client->getFD() );
}

// @brief quit message to send to client
// @params[in] client to send notification of the source quitting
// @params[in] source The client nickname who is quitting
// @params[in] reason The reason why the client source is quitting
// @exceptsafe no-throw
void Quit::quit( const Clients* client, const std::string& source, const std::string& reason ) {
    try { sendReplyTo( client->getNickName(), source, "QUIT", reason ); }
    catch (std::logic_error& ex ) { return ; }
    catch ( RuntimeData::NotFoundException& ex ) { return ; }
}

// @brief quit message to send to a list of clients
// @params[in] clients List of clients to send notifications of the source quiting
// @params[in] source The client nickname who is quitting
// @params[in] reason The reason why the client source is quitting
// @exceptsafe no-throw
void Quit::quit( const std::vector<Clients *>clients, const std::string& source, const std::string& reason ) {
    try { sendReplyToList( clients, source, "QUIT: ", reason ); }
    catch ( std::logic_error& ex ) { return ; }
    catch ( RuntimeData::NotFoundException& ex ) { return ; }
}
