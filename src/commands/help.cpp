/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   help.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:51:10 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/06 01:44:37 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    ERR_HELPNOTFOUND (524)
    RPL_HELPSTART (704)
    RPL_HELPTXT (705)
    RPL_ENDOFHELP (706)
*/

// @brief HELP command
Help::Help( ) : ICommand( ) {

}

Help::~Help( ) {

}

void Help::cmdExecute( void ) {
    std::vector<std::string> params;
    try { params = getParams(); }
    catch ( ICommandException& ex ) { return ; }

    cmdError( ERR_HELPNOTFOUND, params.front() );
}
