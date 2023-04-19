/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:33 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/07 12:34:18 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    RPL_LUSEROP (252)
    RPL_LUSERCLIENT
    RPL_LUSERM
    RPL_LUSERUNKNOWN (253)
    RPL_LUSERCHANNELS (254)
    RPL_LUSERME (255)
    RPL_LOCALUSERS (265)
    RPL_GLOBALUSERS (266)
*/

// @brief CAP command
Cap::Cap( ) : ICommand( ) {

}

Cap::~Cap( ) {
    // Clen up command pass memory
}

void Cap::cmdExecute( void ) {
    Clients* client;
    RuntimeData* rtd;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
        rtd = getRunTimeData();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( client->isRegistered() )
        return ;


    if ( params[0] == "LS")
        sendReply( libft::to_string( SERVER ), "CAP", "", "* LS" );

    else if ( params[0] == "END" ) {
        if ( !client->isAuthenticated() )
            return ;

        std::string nClients = libft::to_string( rtd->getClientsCount() );
        std::string iClients = libft::to_string( rtd->getClientsInvCount() );
        std::string nickName = client->getNickName();

        if ( nickName.empty() )
            return ;

        sendReply( RPL_WELCOME, "Welcome to the " +  libft::to_string( SERVER ) + " Network, " + nickName );
        sendReply( RPL_YOURHOST, "Your host is " + libft::to_string( SERVER ) + ", running version " + libft::to_string( VERSION ) );
        sendReply( RPL_CREATED, "This server was created on " + rtd->getServerStartTime() );

        sendReply( RPL_MYINFO, libft::to_string( SERVER ) + " " + libft::to_string( VERSION ) + " iwro imstn lk" );
        sendReply( RPL_ISUPPORT, "are supported by this server", " NICKLEN=" + libft::to_string( rtd->getNickMaxLength() ) + " MAXCHANNELS=" + libft::to_string( CHANJOINLIMIT ) + " CHANTYPES=#& CHANMODES=,k,l,imstn");
        sendReply( RPL_LUSERCLIENT,  "There are " + nClients + " user(s) and " + iClients + " invisible on 1 server" );
        sendReply( RPL_LUSERME, "I have " + nClients + " client(s) and *this server" );

        sendReply( RPL_MOTDSTART, "Message of the Day" );
        sendReply( RPL_MOTD, "                 " );
        sendReply( RPL_MOTD, "                 " );
        sendReply( RPL_MOTD, "    Keep Swimming" );
        sendReply( RPL_MOTD, "                 " );
        sendReply( RPL_MOTD, "ft_irc is a PAIN in the ***" );
        sendReply( RPL_MOTD, "No Ascii tree are here !!!" );
        sendReply( RPL_MOTD, "                 " );
        sendReply( RPL_ENDOFMOTD, "End of /MOTD command" );

        getClient()->setRegisterd( true );
    }
}
