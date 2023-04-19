/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/28 11:19:41 by hawadh            #+#    #+#             */
/*   Updated: 2023/04/12 12:31:36 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"
#include "../../includes/channels.hpp"

/*
    RPL_TOPIC (332)
    RPL_NAMREPLY (353)
    RPL_ENDOFNAMES (366)

    ERR_NOSUCHCHANNEL (403)
    ERR_TOOMANYCHANNELS (405)
    ERR_NEEDMOREPARAMS (461)
    ERR_CHANNELISFULL (471)
    ERR_INVITEONLYCHAN (473)
    ERR_BADCHANNELKEY (475)
    ERR_BADCHANMASK (476)
*/

Join::Join( void ) {}

Join::~Join( void ) {}

//TODO:             See TODO at bottom
//!                 includes delete at bottom
//@brief            initates the join channel process
//@params[in]       client chanName key
//@exceptsafe       no-throw
void    Join::join( Clients* client, const std::string& chanName, const std::string& key ) {
    if ( !client )
        return ;

    RuntimeData*    rtd;
    try { rtd = getRunTimeData(); }
    catch ( ICommandException& ex ) { return ; }

    if ( client->getChannelCount() >= CHANJOINLIMIT )
        return cmdError( ERR_TOOMANYCHANNELS );

    Channels*       channel;
    bool            newChannel = false;
    try { channel = rtd->getChannel( chanName ); }
    catch ( std::invalid_argument& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }
    catch ( std::runtime_error& ex ) {
        try { channel = rtd->addChannel( chanName ); }
        catch ( std::invalid_argument& ex ) { return cmdError( ERR_BADCHANMASK ); }

        newChannel = true;
    }

    bool    chanNeedInvite = channel->getChanMode() & 8;
    bool    chanNeedKey = channel->getChanMode() & 32;

    if ( !newChannel && chanNeedKey && !chanNeedInvite ) {
        bool    hasKey;

        key.empty() ? hasKey = false : hasKey = true;
        if ( !hasKey )
            return cmdError( ERR_NEEDMOREPARAMS );

        if ( key != channel->getKey() )
            return cmdError( ERR_BADCHANNELKEY );
    }

    try { channel->addClient( client ); }
    catch ( Channels::ModeViolationException& ex ) {
        const std::string& except( ex.what() );

        if ( except == "l" )
            cmdError( ERR_CHANNELISFULL );

        if ( except == "i" )
            cmdError( ERR_INVITEONLYCHAN );
        
        throw ;
    }
    catch ( std::runtime_error& ex ) { return ; }

    if ( newChannel )
        channel->setChanOpMode( client, channel->getChanOpMode( client ) | 2 );

    const std::string& channelName = channel->getName();
    const std::string& clientName = client->getNickName();

    sendReply( clientName, "JOIN", "", channelName );

    std::vector<Clients *> clist = rtd->getChannelList( chanName );
    sendReplyToList( clist, clientName, "JOIN", "", channelName );

    ICommand* icmd = new Topic();

    try {
        icmd->setClient( client );
        icmd->loadRunTimeData( rtd );
    }
    catch ( std::runtime_error& ex ) {
        delete icmd;
        return ;
    }

    ((Topic *)icmd)->topic( client, chanName, "" );

    delete icmd;

    ICommand*   namesReply = new Names();
    namesReply->loadRunTimeData( rtd );
    try { namesReply->setClient( client ); }
    catch ( std::runtime_error& ex ) {  }

    std::vector<std::pair<std::string, unsigned char> > clientsVector;

    try { clientsVector = rtd->getChannelClient( channelName ); }
    catch ( std::invalid_argument& ex ) {
        delete namesReply;
        return cmdError( ERR_NOSUCHCHANNEL );
    }
    catch ( std::runtime_error& ex ) {
        delete namesReply;
        return ;
    }

    ((Names*)namesReply)->names( client, channelName );

    delete namesReply;
}

/*
*   If channel doesn't exist, addClient after creating channel
*   and return as we don't care about channel modes.
*/
//@brief        Adds user to a channel, if no channel, calls addChannel();
//              and sets first user to join as ChanOp. Calls Names command to
//              send NAMREPLY and ENDOFNAMES.
//@exceptsafe   no throw
void    Join::cmdExecute( void ) {
    Clients*    client;
    std::vector<std::string>    params;

    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated()  || !client->isRegistered() )
        return ;

    try { join( client, params.front(), params.back() ); }
    catch ( Channels::ModeViolationException& ex ) { return ; }
    catch ( std::runtime_error& ex ) { return ; }
}
