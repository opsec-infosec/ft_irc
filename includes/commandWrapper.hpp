/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandWrapper.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:52:03 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 23:01:01 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_WRAPPER_HPP
# define COMMAND_WRAPPER_HPP
# include "ICommand.hpp"
# include "runtime.hpp"
# include "commands.hpp"

# include <iostream>
# include <string>

namespace irc {

    //@brief commandWrapper
    // This wraps the irc commands with the required data needed for them to execute
    class commandWrapper {
        public:
            commandWrapper( RuntimeData rtd );
            ~commandWrapper( );

            void cmdExecute( Clients* client);

        private:
            std::string command;
            std::map<std::string, ICommand*> m_cmds;
            ICommand* m_baseCmd;
            Clients* m_client;
            RuntimeData m_runtimeData;
            Logger m_log;

            ICommand* getCmd( const std::string& command );
    };
}
#endif
