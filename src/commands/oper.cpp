/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oper.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/25 00:18:38 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/09 19:26:24 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_PASSWDMISMATCH (464)
    ERR_NOOPERHOST (491)
    RPL_YOUREOPER (381)
*/

// @brief Oper is used to make a client an IRC Operator
Oper::Oper( ) : ICommand( ) {

}

Oper::~Oper( ) {

}

void Oper::cmdExecute( void ) {
    Clients* client;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( params.size() != 2 )
        return cmdError( ERR_NEEDMOREPARAMS );

    if ( params.front() != client->getNickName() || params.back() != libft::to_string( OPERPASS)) {
        return cmdError( ERR_PASSWDMISMATCH );
    }

    if ( client->isOperator() )
        return cmdError( RPL_YOUREOPER, client->getNickName() );

    client->setMode( client->getMode() | 16 );

    sendReply( libft::to_string( SERVER ), "MODE", "", client->getNickName() + " +o" );
    cmdError( RPL_YOUREOPER, client->getNickName() );
}
