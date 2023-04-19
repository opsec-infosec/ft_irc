/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   squit.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/26 18:44:06 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 02:08:24 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   baseCmd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:23 by dfurneau          #+#    #+#             */
/*   Updated: 2023/03/26 01:37:08 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_NOSUCHSERVER (402)
    ERR_NEEDMOREPARAMS (461)
    ERR_NOPRIVILEGES (481)
    ERR_NOPRIVS (723)
*/

// @brief SQUIT Command
//  will basically do nothing
Squit::Squit( ) : ICommand( ) {

}

Squit::~Squit( ) {

}

void Squit::cmdExecute( void ) {
    Clients* client;
    try { client = getClient(); }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    if ( !client->isOperator() ) {
        cmdError( ERR_NOPRIVILEGES );
        return ;
    }

    std::vector<std::string> params;
    try { params = getParams(); }
    catch ( ICommandException& ex ) { return cmdError( ERR_NEEDMOREPARAMS ); }

    std::string trailing = getTrailing();

    squit( params.front(), trailing );
}

void Squit::squit( const std::string& source, const std::string& trailing = "" ) {
    (void)trailing;
    cmdError( ERR_NOSUCHSERVER, source );
}
