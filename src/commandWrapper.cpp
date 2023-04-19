/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commandWrapper.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:51:37 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 23:00:06 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/commandWrapper.hpp"

using namespace irc;

// @brief commandWrapper Constructor
// @params[in] rtd RunTimeData used to hold channel and user lists
commandWrapper::commandWrapper( RuntimeData rtd ) : m_runtimeData( rtd ) {
    m_cmds.insert( std::pair<std::string, ICommand *>( "CAP", new Cap( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "WHO", new Who( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "JOIN", new Join( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "QUIT", new Quit( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "PASS", new Pass( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "NICK", new Nick( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "USER", new User( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "MODE", new Mode( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "PING", new Ping( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "KICK", new Kick( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "KILL", new Kill( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "HELP", new Help( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "PART", new Part( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "OPER", new Oper( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "SQUIT", new Squit( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "TOPIC", new Topic( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "NAMES", new Names( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "REHASH", new Rehash( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "NOTICE", new Notice( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "INVITE", new Invite( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "PRIVMSG", new Privmsg( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "RESTART", new Restart( ) ) );
    m_cmds.insert( std::pair<std::string, ICommand *>( "WALLOPS", new Wallops( ) ) );

    m_baseCmd = new BaseCmd();
}

// @brief commandWrapper Destructor
commandWrapper::~commandWrapper( ) {
    for ( std::map<std::string, ICommand*>::iterator it = m_cmds.begin(); it != m_cmds.end(); ++it ) {
        delete it->second;
    }

    delete m_baseCmd;
}

// @brief Execute Command from client
// @exceptsafe no-throw
void commandWrapper::cmdExecute( Clients* client ) {
    m_client = client;
    std::vector<std::string> commands;

    for ( std::deque<std::vector<std::string> >::iterator mit = m_client->getMsgQR().begin(); mit != m_client->getMsgQR().end(); ++mit ) {
        if ( !mit->empty() ) {
            command = mit->front();

            if ( !command.empty() ) {
                ICommand* icmd = getCmd( command );
                icmd->loadRunTimeData( &m_runtimeData );

                try {
                    icmd->setClient( m_client );
                    icmd->cmdExecute();
                }
                catch (std::runtime_error& ex ) { }

                icmd->clearClient();
            }
        }

        if ( !m_client->getMsgQR().empty() )
            m_client->getMsgQR().pop_front();
    }
}

// @brief getCmd returns a pointer to the command class
// @params[in] command Command to find in map, if not found throws CommandNotFoundException
// @returns[in,out] ICommand pointer to command class
// @exceptsafe no-throw
ICommand*  commandWrapper::getCmd( const std::string& command ) {
    std::map<std::string, ICommand *>::iterator it = m_cmds.find( libft::to_upper( command ) );

    if ( it == m_cmds.end() )
        return m_baseCmd;

    return it->second;
}
