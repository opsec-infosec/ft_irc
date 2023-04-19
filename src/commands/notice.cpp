/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/25 23:46:00 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:00:55 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

// TODO Add Channel Notices

// @brief Notice is used to send messages to clients or channels
Notice::Notice( ) : ICommand( ) {

}

Notice::~Notice( ) {

}

void Notice::cmdExecute( void ) {
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

    if ( params.front()[0] != '#' && params.front()[0] != '&' )
        notice( client, params.front(), trailing );
    else {
        try {
            RuntimeData* rtd = getRunTimeData();
            std::vector<Clients *> clist = rtd->getChannelList( params.front() );
            notice( client, clist, trailing );
        }
        catch ( ICommandException& ex ) { return ; }
        catch ( RuntimeData::NotFoundException& ex ) { return ; }
    }
}

// @brief Send Notice to target
// @params[in] source Source of Notice
// @params[in] target Destination of Notice
// @params[in] message The contents of the Notice
// @exceptsafe no-throw
void Notice::notice( const Clients* source, const std::string& target, const std::string& message ) {
    if ( !source || target.empty() )
        return ;

    if ( target[0] == '#' || target[0] == '&' )
        return ;

    try { sendReplyTo( target, source->getNickName(), "NOTICE", message, target ); }
    catch ( RuntimeData::NotFoundException& ex ) { return ; }
    catch ( std::logic_error& ex ) {
            std::cout << ex.what() << " NOTIC Command" << std::endl;
            return ;
        }
}

// @brief Send Notice to target
// @params[in] source Source of Notice
// @params[in] targets Destinations (list of clients) of Notice
// @params[in] message The contents of the Notice
// @exceptsafe no-throw
void Notice::notice( const Clients* source,  const std::vector<Clients *>& targets, const std::string& message ) {
    if ( !source || targets.empty() )
        return ;

    sendReplyToList( targets, source->getNickName(), "NOTICE", message );
}
