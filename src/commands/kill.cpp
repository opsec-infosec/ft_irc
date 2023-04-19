/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 17:17:16 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 01:53:09 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NOSUCHSERVER (402)
    ERR_NEEDMOREPARAMS (461)
    ERR_NOPRIVILEGES (481)
    ERR_NOPRIVS (723)
*/

// TODO Add notification to others in the channel of quiting
// @brief Kill kills a client by nickname
Kill::Kill( ) : ICommand( ) {

}

Kill::~Kill( ) {

}

void Kill::cmdExecute( void ) {
    Clients* client;
    std::vector<std::string> params;
    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( !client->isOperator() )
        return cmdError( ERR_NOPRIVILEGES );

    std::string trailing = getTrailing();

    std::pair<IReplies::t_reply, std::string> err;
    err = kill(client, params.front(), trailing );
    cmdError( err.first, err.second );
}

// @brief KILL command
// @params[in] client Client sending the request
// @params[in] targetNick Target nick name to send the kill command to
// @params[in] reason Reason for killing the target
// @exceptsafe no-throw
std::pair<IReplies::t_reply, std::string> Kill::kill( Clients* client, const std::string& targetNick, const std::string& reason ) {
    try {
        sendReplyTo( targetNick, client->getNickName(), "KILL", reason, targetNick );

        RuntimeData* rtd = getRunTimeData();
        std::vector<Clients *> cclist = rtd->getChannelList( rtd->getClientFromNick( targetNick ) );
        sendReplyToList( cclist, targetNick, "QUIT", "Killed (" + client->getNickName() + " (" + reason + ")" );

        Clients* kclient = rtd->getClientFromNick( targetNick );
        kclient->markForDeletion( kclient->getFD() );
        kclient->markForKill( kclient->getFD() );
        sendReplyTo( targetNick, "ERROR :Closing Link: " + targetNick + " (Killed (" + client->getNickName() + " (" + reason + ")))\r\n" );
    }
    catch ( RuntimeData::NotFoundException& ex ) { return std::make_pair( ERR_NOSUCHSERVER, targetNick ); }
    catch ( ICommandException& ex ) { return std::make_pair( RPL_NONE, std::string() ); }
    catch ( std::logic_error& ex ) { return std::make_pair( RPL_NONE, std::string() ); }

    return std::make_pair( RPL_NONE, std::string() );
}
