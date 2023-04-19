/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rehash.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/22 16:46:56 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/01 23:44:23 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    RPL_REHASHING (382)
    ERR_NOPRIVILEGES (481)
*/

// @brief Rehash will simulate reloading the config file
Rehash::Rehash( ) : ICommand( ) {

}

Rehash::~Rehash( ) {

}

void Rehash::cmdExecute( void ) {
    Clients* client;
    try { client = getClient(); }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( !client->isOperator() )
        return cmdError( ERR_NOPRIVILEGES );

    cmdError( RPL_REHASHING, "config.yml" );
}
