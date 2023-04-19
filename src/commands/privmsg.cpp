/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/25 20:34:50 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:06:31 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NOSUCHNICK (401)
    ERR_NOSUCHSERVER (402)
    ERR_CANNOTSENDTOCHAN (404)
    ERR_TOOMANYTARGETS (407)
    ERR_NORECIPIENT (411)
    ERR_NOTEXTTOSEND (412)
    ERR_NOTOPLEVEL (413)
    ERR_WILDTOPLEVEL (414)
    RPL_AWAY (301)
*/

// TODO Add Channel Messages, currently sends ERR_CANNOTSENDTOCHAN

// @brief Privmsg is used to send messages to clients or channels
Privmsg::Privmsg( ) : ICommand( ) {

}

Privmsg::~Privmsg( ) {

}

void Privmsg::cmdExecute( void ) {
    Clients* client;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    std::string trailing = getTrailing();

    std::pair<IReplies::t_reply, std::string> err;
    err = privmsg( client, params.front(), trailing );
    return cmdError( err.first, err.second );

}

// @brief Send Private Message to target
// @params[in] source Source of message
// @params[in] target Destination of message
// @params[in] message The contents of the message
// @exceptsafe no-throw
std::pair<IReplies::t_reply, std::string>  Privmsg::privmsg( const Clients* source, const std::string& target, const std::string& message ) {
    if ( !source || target.empty() )
        return std::make_pair( RPL_NONE, std::string() );

    if ( target[0] == '#' || target[0] == '&' ) {  // SEND TO CHANNEL
        RuntimeData* rtd;
        std::vector<Clients *>cclist;
        try {
            rtd = getRunTimeData();
            cclist = rtd->getChannelList( target );

            sendReplyToList( cclist, source->getNickName(), "PRIVMSG", message, target );
        }
        catch ( RuntimeData::NotFoundException& ex ) { return std::make_pair( ERR_NOSUCHCHANNEL, source->getNickName() + " " + target ); }
        catch ( std::invalid_argument& ex ) { return std::make_pair( ERR_NOSUCHNICK, source->getNickName() + " " + target ); }
        catch ( Channels::ModeViolationException& ex ) { return std::make_pair( ERR_CANNOTSENDTOCHAN, source->getNickName() + " " + target ); }
    }
    else {  // SEND TO CLIENT
        try { sendReplyTo( target, source->getNickName(), "PRIVMSG", message, target ); }
        catch ( std::logic_error& ex) { return std::make_pair( RPL_NONE, std::string() ); }
        catch ( RuntimeData::NotFoundException& ex ) { return std::make_pair( ERR_NOSUCHNICK, source->getNickName() + " " + target ); }
    }

    return std::make_pair( RPL_NONE, std::string() );
}

// @brief Send Private Message to target
// @params[in] source Source of message
// @params[in] targets Destinations (list of clients) of message
// @params[in] message The contents of the message
// @exceptsafe no-throw
std::pair<IReplies::t_reply, std::string> Privmsg::privmsg( const Clients* source,  const std::vector<Clients *>& targets, const std::string& message ) {
    if ( !source || targets.empty() )
        return std::make_pair( RPL_NONE, std::string() );

    for ( std::vector<Clients *>::const_iterator it = targets.begin(); it != targets.end(); ++it ) {
        std::string target = (*it.base())->getNickName();

        try { sendReplyTo( target, source->getNickName(), "PRIVMSG", message, target ); }
        catch ( std::logic_error& ex ) { return std::make_pair( RPL_NONE, std::string() ); }
        catch ( RuntimeData::NotFoundException & ex ) { }
    }

    return std::make_pair( RPL_NONE, std::string() );
}
