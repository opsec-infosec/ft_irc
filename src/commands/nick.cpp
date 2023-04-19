/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:51:20 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/11 10:13:33 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NONICKNAMEGIVEN (431)
    ERR_ERRONEUSNICKNAME (432)
    ERR_NICKNAMEINUSE (433)
    ERR_NICKCOLLISION (436)
*/

// @brief NICK command
Nick::Nick( ) : ICommand( ) {

}

Nick::~Nick( ) {

}

void Nick::cmdExecute( void ) {
    Clients* client;
    RuntimeData* rtd;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
        rtd = getRunTimeData();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() )
        return ;


    std::string nickName = params.front();
    std::string oldNickName = client->getNickName();

    if ( nickName.empty() )
        return cmdError( ERR_NONICKNAMEGIVEN );

    if ( nickName == oldNickName )
        return ;

    for ( std::string::iterator it = nickName.begin(); it != nickName.end(); ++it ) {
        if ( *it == ' ' || *it == ',' || *it == '*' || *it == '?' || *it == '!' || *it == '.' || it[0] == '$' || it[0] == ':' || it[0] == '#' || it[0] == '&' )
            return cmdError( ERR_ERRONEUSNICKNAME );
    }

    if ( nickName.size() > rtd->getNickMaxLength() )
        nickName.resize( rtd->getNickMaxLength() );

    try { rtd->getClientFromNick( nickName ); }
    catch ( std::logic_error& ex) { return ; }
    catch ( RuntimeData::NotFoundException& ex ) {
        client->setNickName( nickName );
        sendReply( oldNickName, getCmd(), "changed their nickname to " + nickName, nickName );

        std::vector<Clients *> clist = rtd->getChannelList( client );

        if ( clist.empty() )
            return ;

        sendReplyToList( clist, oldNickName, getCmd(), "changed their nickname to " + nickName, nickName );
        return ;
    }

    if ( oldNickName.empty() ) {
        for ( int i = 1000; i != 0; i-- ) {
            srand( std::time( NULL ) );

            const std::string&  append  = libft::to_string( rand() % 1000 );
            if ( nickName.size() >= rtd->getNickMaxLength() )
                nickName.resize( ( nickName.length() - append.length() ) - 1 );

            std::string newNick = nickName + "_" + append;

            try { rtd->getClientFromNick( newNick ); }
            catch ( std::logic_error& ex ) { return ; }
            catch ( RuntimeData::NotFoundException& ex ) {
                client->setNickName( newNick );
                sendReply( oldNickName, getCmd(), "changed their nickname to " + client->getNickName(), client->getNickName() );
                return ;
            }
        }

        close( client->getFD());
        return ;
    }

    cmdError( ERR_NICKNAMEINUSE, nickName );
}
