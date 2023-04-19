/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/30 16:01:24 by hawadh            #+#    #+#             */
/*   Updated: 2023/04/07 17:47:27 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

Names::Names( void ) {}

Names::~Names( void ) {}

/*
    RPL_NAMREPLY (353)
    RPL_ENDOFNAMES (366)
*/

/*
*   Loops through pair and adds modes for each nickname found if mode set
*   if +v (voice mode) set on channel, it will place '+' at either index 0
*   of tmp or at index after '@' if user is chanop, rest will come after
*/
//@brief        lists names of users in channel
//@params[in]   client params
//              chanName Channel name and clientName
//exceptsafe    no-throw
void    Names::names( Clients* client, const std::string& chanName ) {
    RuntimeData*    rtd;
    Channels*       channel;
    try {
        rtd = getRunTimeData();
        channel = rtd->getChannel( chanName );
    }
    catch ( RuntimeData::NotFoundException& ex ) { return cmdError( ERR_NOSUCHCHANNEL ); }
    catch ( std::runtime_error& ex ) { return ; }

    const std::string& clientName = client->getNickName();
    std::vector<std::pair<std::string, unsigned char> > clientsVector;

    try { clientsVector = rtd->getChannelClient( chanName ); }
    catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHCHANNEL ); }
    catch ( std::runtime_error& ex ) { return ; }

    bool    isSecret;
    bool    inChannel;

    channel->getChanMode() & 128 ? isSecret = true : isSecret = false;
    channel->isClientInChannel( client ) ? inChannel = true : inChannel = false;

    if ( chanName.empty() || clientName.empty() || ( isSecret && !inChannel ) )
        return sendReply( RPL_ENDOFNAMES, "End of /NAMES list.", clientName, chanName );

    std::string tmp;
    std::string namesReply;
    std::vector<std::pair<std::string, unsigned char> >::const_iterator it;

    for ( it = clientsVector.begin(); it != clientsVector.end(); it++ ) {
        const std::string    clientMode( "+@%&~" );

        for ( int i = 1; i < 5; i++ )
            if ( it->second & ( 1 << i ) )
                tmp += clientMode.at( i );

        if ( it->second & ( 1 << 0 ) ) {
            size_t  found = tmp.find( '@' );
            
            if ( found != std::string::npos )
                tmp.insert( found + 1, 1, clientMode.at( 0 ) );
            else
                tmp.insert( 0, 1, clientMode.at( 0 ) );
        }

        tmp += it->first;

        if ( clientsVector.size() > 1  && it != clientsVector.end() - 1)
            tmp += " ";
        
        namesReply += tmp;
        tmp.clear();
    }

    sendReply( RPL_NAMREPLY, namesReply, clientName, "= " + chanName );
    sendReply( RPL_ENDOFNAMES, "End of /NAMES list.", clientName, chanName );
}

//@brief        calls names(); method to list name sof clients in channel
//except-safe   no-throw
void    Names::cmdExecute( void ) {
    Clients*  client;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated()  || !client->isRegistered() )
        return ;

    names( client, params.front() );
}
