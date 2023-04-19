/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/01 11:14:45 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:25:15 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_NOSUCHCHANNEL (403)
    ERR_NOTONCHANNEL (442)
*/

// @brief Part exits from a channel
Part::Part( ) : ICommand( ) {

}

Part::~Part( ) {

}

void Part::cmdExecute( void ) {

    Clients* client;
    std::vector<std::string> params;
    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    std::string trailing = getTrailing();

    part( client, params.front(), trailing );
}


void Part::part( Clients* source, const std::string& target, const std::string& reason ) {

    if ( !source )
        return ;

    try {
        RuntimeData* rtd;
        rtd = getRunTimeData();

        Channels* chan;
        chan = rtd->getChannel( target );

        if ( !chan->isClientInChannel( source ) ) {
            cmdError( ERR_NOTONCHANNEL, target );
            return ;
        }

        std::vector<Clients *> cclist = rtd->getChannelList( target );

        std::string trailing = "Leaving";
        if ( !reason.empty() )
            trailing = reason;

        sendReply( source->getNickName(),  "PART", trailing, target );
        sendReplyToList( cclist, source->getNickName(), "PART", trailing, target );

        chan->delClient( source );
    }
    catch( ICommandException& ex) { return ; }
    catch ( RuntimeData::NotFoundException& ex ) { cmdError( ERR_NOSUCHCHANNEL, target ); }
    catch ( std::invalid_argument& ex ) { return ; }
}
