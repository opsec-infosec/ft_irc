/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:51:01 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/01 15:14:14 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NEEDMOREPARAMS (461)
    ERR_ALREADYREGISTERED (462)
    ERR_PASSWDMISMATCH (464)
*/

// @brief PASS Command.  Check for authenticated user is done in CAP command
Pass::Pass( ) : ICommand( ) {

}

Pass::~Pass( ) {
    // Clen up command pass memory
}

void Pass::cmdExecute( void ) {
    Clients* client;
    RuntimeData* rtd;
    std::vector<std::string> params;

    try {
        client = getClient();
        params = getParams();
        rtd = getRunTimeData();
    }
    catch ( ICommandException& ex ) { return ; }

    if ( client->isAuthenticated() )
        return cmdError( ERR_ALREADYREGISTERED );

    std::string password = params.front();
    if ( password != rtd->getPassword() ) {
        cmdError( ERR_PASSWDMISMATCH );
        return ;
    }

    client->setAuthenticated( true );
}
