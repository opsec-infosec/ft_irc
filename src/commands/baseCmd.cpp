/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   baseCmd.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:23 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/02 16:16:58 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
 ERR_UNKNOWNCOMMAND
*/

// @brief BaseCmd empty class non searcheable, returned if command not found
//  will basically do nothing and send the proper error message to client
BaseCmd::BaseCmd( ) : ICommand( ) {

}

BaseCmd::~BaseCmd( ) {

}

void BaseCmd::cmdExecute( void ) {

    // Clients* client;
    // RuntimeData* rtd;
    // std::vector<std::string> params;
    // std::string trailing = getTrailing();

    // try {
    //     client = getClient();   // Always return this is fatal
    //     rtd = getRunTimeData(); // Always return this is fatal
    //     params = getParams();   // Optional return,  if you plan on dereferencing or using subscripts, you need to check the size. don't rely on the try catch.
    // }
    // catch ( ICommandException& ex ) { return ; }

    //(void)rtd;
    //(void)client;
    //(void)params;
    //(void)trailing;


    cmdError( ERR_UNKNOWNCOMMAND );
}

// You can Override the cmdError if you need special replies otherwise it will call the base cmdError
// void BaseCmd::cmdError( IReplies::t_reply errNumber, std::string data ) {
//     sendReply( errNumber, "Unknown Command", this->getCmd() );
//     (void)data;
// }
