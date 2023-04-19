/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   who.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/02 16:03:24 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/13 01:50:34 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    RPL_WHOREPLY (352)
    RPL_ENDOFWHO (315)
    ERR_NOSUCHSERVER (402)
*/

// @brief WHO command is used to query a list of users who match the provided mask.
Who::Who( ) : ICommand( ) {

}

Who::~Who( ) {

}

void Who::cmdExecute( void ) {

    Clients* client;
    std::vector<std::string> params;

    try {
        client = getClient();   // Always return this is fatal
        params = getParams();   // Optional return,  if you plan on dereferencing or using subscripts, you need to check the size. don't rely on the try catch.
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated()  || !client->isRegistered() )
        return ;

    who( client, params.front() );
}

std::string Who::chOp( const unsigned char& chOpMode ) const {
    static const char mode[5] = { '+', '@', '%', '&', '-' };
    std::string ret;

    for ( int i = 0; i < 5; i++ ) {
        if ( chOpMode & ( 1 << i ) )
            ret = mode[i];
    }
    return ret;
}


void Who::who( const Clients* client, const std::string& mask ) {

    if ( !client || mask.empty() )
        return;


    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch( ICommandException& ex ) { return ; }

    if ( mask[0] == '#' || mask[0] == '&' ) {
        Channels* channel;
        std::vector<Clients *> fClients;


        try {
            channel = rtd->getChannel( mask );
            fClients = rtd->getChannelList( mask );
        }
        catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHSERVER, mask ); }
        catch ( RuntimeData::NotFoundException& ex ) { return sendReply( RPL_ENDOFWHO, "End of /WHO list.", mask ); }

        for ( std::vector<Clients *>::const_iterator it = fClients.begin(); it != fClients.end(); ++it ) {
            Clients* c = (*it.base());
            if ( channel->isClientInChannel( client ) || ( !channel->isClientInChannel( client ) && ( ! ( c->getMode() & 2 ) && ! ( channel->getChanMode() & 128 ) ) ) ) {
                sendReply( RPL_WHOREPLY, "0 " + c->getRealName(), client->getNickName(),
                    mask + " "
                    + c->getUserName() + " "
                    + c->getHostName() + " "
                    + c->getConnHostName() + " "
                    + c->getNickName() + " "
                    + "H"
                    + ( ( client->getMode() & 16 ) ? "*" : "" )
                    + chOp( channel->getChanOpMode( c ) )
                );
            }
        }
    }
    else {
        Channels* channel;
        Clients* fClient;

        try { fClient = rtd->getClientFromNick( mask ); }
        catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHSERVER, mask ); }
        catch ( RuntimeData::NotFoundException& ex ) { return sendReply( RPL_ENDOFWHO, "End of /WHO list.", mask ); }

        try {
            channel = NULL;
            channel = rtd->getChannel( fClient->getLastJoinChan() );
        }
        catch ( std::invalid_argument& ex ) { }
        catch ( RuntimeData::NotFoundException& ex ) { }

        sendReply( RPL_WHOREPLY, "0 " + fClient->getRealName(), client->getNickName(),
            ( channel ? ( channel->getChanMode() & 128 ? "* " : fClient->getLastJoinChan() + " " ) : "* " )
            + "~" + fClient->getUserName() + " "
            + fClient->getHostName() + " "
            + fClient->getConnHostName() + " "
            + fClient->getNickName() + " "
            + "H"
            + ( ( fClient->getMode() & 16 ) == 16 ? "*" : "" )
            + ( channel ? ( chOp( channel->getChanOpMode( fClient ) ) ) : "" )
        );
    }
    sendReply( RPL_ENDOFWHO, "End of /WHO list.", mask );
}
