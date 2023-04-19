/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ICommand.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/26 20:07:51 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 23:02:13 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/ICommand.hpp"

using namespace irc;

//@brief Base Command ABC, inherit from this base class to implement commands
ICommand::ICommand( ) : m_client( NULL ), m_rtd( NULL ) { }
ICommand::~ICommand( ) { }

void ICommand::cmdError( const IReplies::t_reply& errNumber, const std::string& data ) {
    switch ( static_cast<int>( errNumber ) ) {
        case ERR_NOPRIVILEGES:
            sendReply( errNumber, "Permission Denied -Not an IRC Operator " );
            break;

        case ERR_CHANOPRIVSNEEDED:
            sendReply( errNumber, "You're not channel operator", data );
            break;

        case ERR_UNKNOWNCOMMAND:
            sendReply( errNumber, "Unknown Command", m_command );
            break;

        case ERR_UMODEUNKNOWNFLAG:
            sendReply( errNumber, "Unknown MODE flag" );
            break ;

        case ERR_NEEDMOREPARAMS:
            sendReply( errNumber, data + "Need More Parameters", m_command );
            break;

        case ERR_NOSUCHNICK:
            sendReply( errNumber, "No such nick", data );
            break;

        case ERR_USERSDONTMATCH:
            sendReply( errNumber, "Cant change mode for other users", data );
            break;

        case ERR_NONICKNAMEGIVEN:
            sendReply( errNumber, "No Nick Name Given" );
            break ;

        case ERR_USERNOTINCHANNEL:
            sendReply( errNumber, "They aren't in that Channel" );
            break ;

        case ERR_ERRONEUSNICKNAME:
            sendReply( errNumber, "Nickname Contains Illegal Characters", m_command );
            break ;

        case ERR_NICKNAMEINUSE:
        case ERR_NICKCOLLISION:
            sendReply( errNumber, data, "", "Nick Name already in use" );
            break ;

        case ERR_NOSUCHCHANNEL:
            sendReply( errNumber, "No Such Channel", data );
            break;

        case ERR_NOTONCHANNEL:
            sendReply( errNumber, "You're not on that channel", data );
            break;

        case ERR_PASSWDMISMATCH:
            sendReply( errNumber, "Password incorrect" );
            break;

        case RPL_YOUREOPER:
            sendReply( RPL_YOUREOPER, "You are now a IRC Operator", data );
            break;

        case ERR_ALREADYREGISTERED:
            sendReply( errNumber, "You may not change PASS password" );
            break;

        case ERR_TOOMANYCHANNELS:
            sendReply( errNumber, "You have joined too many channels", data );
            break;

        case ERR_CANNOTSENDTOCHAN:
            sendReply( errNumber, "Cannot send to channel", data );
            break;

        case ERR_USERONCHANNEL:
            sendReply( errNumber, "Already on channel", data );
            break;

        case ERR_HELPNOTFOUND:
            sendReply( ERR_HELPNOTFOUND, "No help available on this topic", data );
            break;

        case ERR_BADCHANMASK:
            sendReply( ERR_BADCHANMASK, "Bad Channel Mask", data );
            break;

        case ERR_BADCHANNELKEY:
            sendReply( ERR_BADCHANNELKEY, "Cannot join channel (+k)", data );
            break;

        case ERR_CHANNELISFULL:
            sendReply( ERR_CHANNELISFULL, "Cannot join channel (+l)", data );
            break;

        case ERR_INVITEONLYCHAN:
            sendReply( ERR_INVITEONLYCHAN, "Cannot join channel (+i)", data );
            break;

        case ERR_INVALIDKEY:
            sendReply( ERR_INVALIDKEY, "Key is not well-formed", data );
            break;

        case ERR_INVALIDMODEPARAM:
            // This Error should be send as a Sendreply as it has specific parameters
            break;

        case RPL_REHASHING:
            sendReply( RPL_REHASHING, "Rehashing", data );
            break;

        default:
            break;

    }
}

// @brief setClient Set the client requesting the command and loads into the ICommand interface
//      Used with command wrapper, do not call directly
// @params[in, out] client Pointer to client class
// @throws std::runtime_error
// @exceptsafe strong
void ICommand::setClient( Clients* client ) {
    m_client = client;

    for ( std::deque<std::vector<std::string> >::iterator it = m_client->getMsgQR().begin(); it != m_client->getMsgQR().end(); ++it ) {
        int msgSize = 0;
        for ( size_t i = 0; i < it->size(); i++) {
            msgSize += it->data()[i].size();
        }
        // include /r/n
        if ( msgSize + 2 > 512 ) {
            if ( m_client->isAuthenticated() ) {
                sendReply( ERR_INPUTTOOLONG, "input line too long" );
                throw std::runtime_error("Message Size Exceeded 512 Bytes");
            }
        }
    }

    if ( m_client &&  m_client->getMsgQR().size() ) {
        m_command = m_client->getMsgQR().front().front();
    }

    if ( m_client && m_client->getMsgQR().size() ) {
        for ( std::vector<std::string>::iterator it = m_client->getMsgQR().front().begin(); it != m_client->getMsgQR().front().end(); ++it ) {
            if ( it != m_client->getMsgQR().front().begin() && ( *it )[0] != ':' )
                m_params.push_back( *it );
        }
    }

    if ( m_client && m_client->getMsgQR().size() ) {
        if ( m_client->getMsgQR().front().back().size() > 1  && m_client->getMsgQR().front().back()[0] == ':')
            m_trailing = m_client->getMsgQR().front().back().substr( 1 );
    }
}

// @brief Clears up all variables in ICommand
// @exceptsafe no-throw
void ICommand::clearClient( void ) {
    if ( !m_params.empty() )
        m_params.erase( m_params.begin(), m_params.end() );

    m_client = NULL;
    m_rtd = NULL;
    m_command = "";
    m_trailing = "";
}

// @brief Load the RuntTimeData from the server
// @params[in] rtd RunTimeData class
// @exceptsafe no-throw
void ICommand::loadRunTimeData( RuntimeData* rtd ) {
    m_rtd = rtd;
}

// @brief sendReply Raw Send Method
// @param[in] rply std::string to send raw message
// @exceptsafe no-throw
void ICommand::sendReply( const std::string& rply ) {
    if ( !m_client )
        return ;

    m_client->setMsgQS( rply );
}

// @brief sendReply Error ( :ft_irc errNumber [target] [operation] [:trailing] )
// @param[in] errNumber std::IReplies::t_type Error Number to send
// @param[in] trailing std::string trailing parameters
// @param[in] target std::string target of the error, optional
// @param[in] operations std::string additional info, optional
// @exceptsafe no-throw
void ICommand::sendReply( const IReplies::t_reply& errNumber,
    const std::string& trailing,
    const std::string& target,
    const std::string& operations ) {
    if ( !m_client )
        return ;

    std::string tmp;
    std::stringstream ss;

    ss << std::setfill('0') << std::setw(3) << errNumber;

    if ( !operations.empty() && !target.empty() && !trailing.empty() )
        tmp = std::string( ":" + libft::to_string( SERVER ) + " " + ss.str()  + " " + target + " " + operations + " :" + trailing + "\r\n" );
    else if ( operations.empty() && !target.empty() && !trailing.empty() )
        tmp = std::string( ":" + libft::to_string( SERVER ) + " " + ss.str() + " " + target + " :" + trailing + "\r\n" );
    else if ( !operations.empty() && !target.empty() && trailing.empty() )
        tmp = std::string( ":" + libft::to_string( SERVER ) + " " + ss.str() + " " + target + " " + operations +"\r\n" );
    else
        tmp = std::string( ":" + libft::to_string( SERVER ) + " " + ss.str() + " " + getClient()->getNickName() + " :" + trailing + "\r\n" );

    m_client->setMsgQS( tmp );
}

// @brief sendReply ( :ft_irc source command [target] [:trailing] )
// @param[in] source std::string first arg after :server usually the client nickname, this arg is required
// @param[in] command std::string second arg after source is the command, this arg is required
// @param[in] target std::string third arg after command is the target of the commmand, this arg is optional but must send an empty string ( std::string() )
// @param[in] trailing std::string fourth arg after target is the trailing parameters, this arg is optional
// @exceptsafe no-throw
void ICommand::sendReply( const std::string& source, const std::string& command, const std::string& trailing, const std::string& target ) {
    if ( !m_client )
        return ;

    std::string tmp;

    if ( !target.empty() &&  !trailing.empty() )
        tmp = ( ":" + source + " " + command + " " + target + " :" + trailing + "\r\n" );
    else if ( target.empty() && !trailing.empty() )
        tmp = ( ":" + source + " " + command + " :" + trailing + "\r\n" );
    else
        tmp = ( ":" + source + " " + command + " " + target + "\r\n" );

    m_client->setMsgQS( tmp );
}

// @brief sendReplyTo Raw Send Method
// @param[in] nickName message to send reply to
// @param[in] rply std::string to send raw message
// @throws RuntimeData::NotFoundException if nickName not found
// @exceptsafe strong
void ICommand::sendReplyTo( const std::string& nickName, const std::string& rply ) {
    if ( nickName.empty() )
        return ;

    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch ( ICommandException& ex ) { return ; }

    Clients* fClient = rtd->getClientFromNick( nickName );

    Clients* client = this->m_client; //Temp store the current client
    m_client = fClient; //reassign the found client to this m_client
    sendReply( rply );
    m_client = client; //restore original current client
}

// @brief sendReplyTo ( :ft_irc source command [target] [:trailing] )
// @params[in] nickName nick name to send the reply message to
// @params[in] source std::string first arg after :server usually the client nickname, this arg is required
// @params[in] command std::string second arg after source is the command, this arg is required
// @params[in] target std::string third arg after command is the target of the commmand, this arg is optional but must send an empty string ( std::string() )
// @params[in] trailing std::string fourth arg after target is the trailing parameters, this arg is optional
// @throws RuntimeData::NotFoundException on nickName/channel not found
// @throws Channels::ModeViolationException if cannot send to channel, check ex.what() for ChanMode that is violated
// @throws std::logic_error if runtimedata is not loaded
// @exceptsafe strong
void ICommand::sendReplyTo( const std::string& nickName, const std::string& source, const std::string& command, const std::string& trailing, const std::string& target ) {
    if ( nickName.empty() )
        return ;

    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch ( ICommandException& ex ) { return ; }

    // Send to Client
    Clients* fClient = rtd->getClientFromNick( nickName );
    std::string cSource = ( source.empty() ? m_client->getNickName() : source );

    Clients* client = this->m_client; //Temp store the current client
    m_client = fClient; //reassign the found client to this m_client
    sendReply( cSource, command, trailing, target );
    m_client = client; //restore original current client
}

// @brief sendReplyToList ( :ft_irc source command [target] [:trailing] )
// @params[in] clientList of client records to send reply message to
// @params[in] source std::string first arg after :server usually the client nickname, this arg is required
// @params[in] command std::string second arg after source is the command, this arg is required
// @params[in] target std::string third arg after command is the target of the commmand, this arg is optional but must send an empty string ( std::string() )
// @params[in] trailing std::string fourth arg after target is the trailing parameters, this arg is optional
// @throws
// @exceptsafe no-throw
void ICommand::sendReplyToList( const std::vector<Clients *>& clientList, const std::string& source, const std::string& command, const std::string& trailing, const std::string& target ) {
    if ( clientList.empty() )
        return ;

    for ( std::vector<Clients *>::const_iterator it = clientList.begin(); it != clientList.end(); ++it ) {
        if ( m_client != *it.base() ) {
                // Send to Channel
            if ( !target.empty() && !source.empty()  && ( target[0] == '#' || target[0] == '&' ) ) {
                Channels* channel;
                Clients* fClient;

                try {
                    fClient = m_rtd->getClientFromNick( (*it.base())->getNickName() );
                    channel = m_rtd->getChannel( target );
                    channel->chanMsg( fClient, source, command, trailing, target );
                }
                catch ( RuntimeData::NotFoundException& ex ) {  }
                catch( Channels::ModeViolationException& ex ) { throw ; }
            }
            else {
                try { sendReplyTo( (*it.base())->getNickName(), source, command, trailing, target.empty() ? (*it.base())->getNickName() : target ); }
                catch ( std::logic_error& ex ) { return ; }
                catch ( RuntimeData::NotFoundException& ex ) { }
            }
        }
    }
}

// @brief Reply To List with errnumber and raw
// @params[in] clientList of client records to send reply message to
// @params[in] client client sending the message, not used
// @params[in] errNumber Errornumber to send
// @params[in] reply raw message
// @exceptsafe no-throw
void ICommand::sendReplyToList( const std::vector<Clients *>& clientList, const Clients* client, const std::string& errNumber, const std::string& reply ) {
    if ( clientList.empty() )
        return ;

    for ( std::vector<Clients *>::const_iterator it = clientList.begin(); it != clientList.end(); ++it ) {
        if ( m_client != *it.base() ) {
                (void)client;
                try { sendReplyTo( (*it.base())->getNickName(), libft::to_string( SERVER ) + " " + errNumber,  (*it.base())->getNickName() + " " + reply ); }
                catch ( std::logic_error& ex ) { return ; }
                catch ( RuntimeData::NotFoundException& ex ) { }

        }
    }
}


// @brief sendReplyToLIst ( :ft_irc ERRNUMBER trailing target operations )
// @params[in] clientList of client records to send reply message to
// @params[in] trailing trailing parameter
// @params[in] target target of the message
// @params[in] operations final optional argument
// @exceptsafe no-throw
void ICommand::sendReplyToList( const std::vector<Clients *>& clientList,
    const IReplies::t_reply& errNumber,
    const std::string& trailing,
    const std::string& target,
    const std::string& operations,
    const std::string& source ) {
    if ( clientList.empty() )
        return ;

    for ( std::vector<Clients *>::const_iterator it = clientList.begin(); it != clientList.end(); ++it ) {
        if ( m_client != *it.base() ) {
            if ( !operations.empty() && ( operations[0] == '#' || operations[0] == '&' ) ) {
                Channels* channel;
                Clients* fClient;

                try {
                    fClient = m_rtd->getClientFromNick( (*it.base())->getNickName() );
                    channel = m_rtd->getChannel( operations );
                    if ( source.empty() )
                        channel->chanMsg( fClient, fClient->getNickName(), libft::to_string( errNumber ), trailing, target, operations );
                    else
                        channel->chanMsg( fClient, source, libft::to_string( errNumber ), trailing, target, operations );
                }
                catch ( RuntimeData::NotFoundException& ex ) { }
                catch ( Channels::ModeViolationException& ex ) { }
            }
            else {
                try { sendReply( errNumber, trailing, target, operations ); }
                catch ( std::logic_error& ex ) { return ; }
                catch ( RuntimeData::NotFoundException& ex ) { }
            }
        }
    }
}

// @brief Close the file descriptor FD.
//  This function is a cancellation point and therefore not marked with __THROW.
// @params[in] fd File Descriptor
// @exceptsafe no-throw
void ICommand::close( const int& fd ) {
    if ( m_client ) {
        if ( m_client->getFD() == fd )
            m_client->markForDeletion( fd );
        else {
            Clients* client;
            try { client = m_rtd->getClientFromFD( fd ); }
            catch ( RuntimeData::NotFoundException& ex) { return ; }
            client->markForDeletion( fd );
        }
    }
}

// @brief Get the Current Command as a string
// @returns[out] std::string command
// @exceptsafe no-throw
std::string ICommand::getCmd( void ) {
    return m_command;
}

///!!! NEED TO VERIFY THAT ALL COMMANDS HAVE PARAMETERS !!!
// @brief getCmd Gets the Parameters of the command part of the irc protocol
// @returns[out] std::vector<std::string> Parameters
// @throws ICommandException on empty parameters vector
// @exceptsafe strong
const std::vector<std::string>& ICommand::getParams( void ) {
    if ( !m_params.size() ) {
        sendReply( ERR_NEEDMOREPARAMS, "Need more parameters" );
        throw ICommandException( m_command + " - vector empty" );
    }

    return m_params;
}

// @brief get Trailing message ( :xxxx ) paramaters
// @returns[out] std::string trailing parameters
// @exceptsafe no-throw
const std::string& ICommand::getTrailing( void ) {
    return m_trailing;
}

// @brief getClient Gets the current Client class loaded in the command
// @returns[out] Clients* class
// @throws ICommandException on NULL Client pointer
// @exceptsafe basic
Clients* ICommand::getClient( void ) {
    if ( m_client )
        return m_client;

    throw ICommandException( m_command + " - Fatal Error empty Client");
}

// @brief getRunTimeData Gets the runtime data from the server, channel classes, all client classes that are  connected
// @returns[out] RuntimeData* class
// @throws ICommandException on NULL Runtime pointer
// @exceptsafe basic
RuntimeData* ICommand::getRunTimeData( void ) {
    if ( m_rtd )
        return m_rtd;

    if ( m_client ) {
        sendReply( "ERROR: Fatal Error: " + libft::to_string( SERVER ) + "\r\n" );
        close( m_client->getFD() );
    }

    throw ICommandException( m_command + " - Fatal Error empty RunTimeData");
}
