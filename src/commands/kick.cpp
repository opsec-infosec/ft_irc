/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/27 16:13:25 by hawadh            #+#    #+#             */
/*   Updated: 2023/04/07 17:17:53 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"


/*
    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_CHANOPRIVSNEEDED (482)
    ERR_USERNOTINCHANNEL (441)
    ERR_NOTONCHANNEL (442)
    ERR_NOSUCHNICK (401)
*/

//  TODO:   Send notification to all users in channel.
//* Message: <initiating user> <command> <ChannelName> <TargetUser> :<optionalReason>
/*
!   Not Recommended to handle multiple user kick as it
!   is not Portable.

!   IRC Ops cannot control channels they are not ChanOps on
*/

Kick::Kick( ): ICommand( ) {}

Kick::~Kick( ) {}

void    Kick::kick( Clients* client, Channels* channel, const std::string& kickTarget, const std::string& reason ) {
    const std::string&  clientName = client->getNickName();
    const std::string&  channelName = channel->getName();
    if ( clientName.empty() || channelName.empty() )
        return cmdError( ERR_NEEDMOREPARAMS );

    RuntimeData*    rtd;
    try { rtd = getRunTimeData(); }
    catch ( std::runtime_error& ex ) { return ; } 
    
    try {
        std::vector<Clients *> cclist = rtd->getChannelList( rtd->getClientFromNick( kickTarget ) );
        std::string isReason;
        reason.empty() ? isReason = clientName : isReason = reason;
        sendReplyToList( cclist, clientName, "KICK " + channelName, isReason, kickTarget );
        sendReply( clientName, "KICK " + channelName, isReason, kickTarget );

        Clients*    target = rtd->getClientFromNick( kickTarget );
        channel->delClient( target );

    } 
    catch ( std::runtime_error& ex ) { return cmdError( ERR_USERNOTINCHANNEL ); }
    catch ( std::logic_error& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }
}

//  @brief Kicks a user from channel by nickname
void    Kick::cmdExecute( void ) {
    Clients*    client;
    try{ client = getClient(); }
    catch ( std::runtime_error& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    std::vector<std::string>    params;
    try { params = getParams(); }
    catch ( std::runtime_error& ex ) { return ; }

    if ( params.empty() || params.size() < 2 )
        return cmdError( ERR_NEEDMOREPARAMS );

    RuntimeData*    rtd;
    try { rtd = getRunTimeData(); }
    catch ( std::runtime_error& ex ) { return ; } 
    
    Channels*       channel;
    try { channel = rtd->getChannel( params.front() ); }
    catch ( std::runtime_error& ex ) { return cmdError( ERR_NOSUCHCHANNEL ); }
    catch ( std::logic_error& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }

    if ( ! ( channel->getChanOpMode( client ) & 2 ) )
        return cmdError( ERR_CHANOPRIVSNEEDED );

    const std::string&  trailing = getTrailing();
    kick( client, channel, params.back(), trailing );
}
