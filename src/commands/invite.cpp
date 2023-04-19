/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/01 20:15:34 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 23:18:18 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    RPL_INVITING (341)
    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_NOTONCHANNEL (442)
    ERR_CHANOPRIVSNEEDED (482)
    ERR_USERONCHANNEL (443)
*/

// @brief Invite is used to invite clients to a channel
//   the priority is givent to invites, then keys
Invite::Invite( ) : ICommand( ) {

}

Invite::~Invite( ) {

}

void Invite::cmdExecute( void ) {

    Clients* client;
    RuntimeData* rtd;
    std::vector<std::string> params;
    std::string trailing = getTrailing();

    try {
        client = getClient();   // Always return this is fatal
        rtd = getRunTimeData(); // Always return this is fatal
        params = getParams();   // Optional return,  if you plan on dereferencing or using subscripts, you need to check the size. don't rely on the try catch.
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() && !client->isRegistered() )
        return ;

    if ( params.size() < 2 )
        return cmdError( ERR_NEEDMOREPARAMS );



    Channels* chan;
    Clients* fClient;
    std::string chanName;

    try { fClient = rtd->getClientFromNick( params.front() ); }
    catch ( RuntimeData::NotFoundException& ex ) { return cmdError( ERR_NOSUCHNICK, params.front() ); }

    try {
        chan = rtd->getChannel( params.back() );
        chanName = chan->getName();

        if ( !chan->isClientInChannel( client ) )
            return cmdError( ERR_NOTONCHANNEL, chanName );

        if ( chan->getChanMode() & 8 && !( chan->getChanOpMode( client ) & 2 ) )
            return cmdError( ERR_CHANOPRIVSNEEDED, chanName );

        if ( chan->isClientInChannel( fClient ) )
            return cmdError( ERR_USERONCHANNEL, fClient->getNickName() + " " + chanName );

    }
    catch ( RuntimeData::NotFoundException& ex ) { return cmdError( ERR_NOSUCHCHANNEL , chanName ); }

    invite ( fClient, chan );
}


void Invite::invite( Clients* targetClient, Channels* channel ) {
    Clients* client = getClient();
    std::string nickName = targetClient->getNickName();

    channel->addClientInvite( targetClient );

    sendReply( ":" + libft::to_string( SERVER )+ " " + libft::to_string( RPL_INVITING ) + " " + client->getNickName() + " " + nickName + " " + channel->getName() + " \r\n" );
    sendReplyTo( nickName, ":" + client->getNickName() + " " + "INVITE" + " " + nickName + " " +  channel->getName() + "\r\n" );
}


void Invite::invite( const std::string& nickName, const std::string& channelName ) {
    if ( nickName.empty() || channelName.empty() )
        return ;

    Clients* fClient;
    Channels* chan;
    RuntimeData* rtd;

    try {
        rtd = getRunTimeData();
        fClient = rtd->getClientFromNick( nickName );
        chan = rtd->getChannel( channelName );
    }
    catch ( ICommandException& ex ) { return ; }

    invite( fClient, chan );
}
