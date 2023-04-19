/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/20 20:45:56 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/14 18:17:27 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"

using namespace irc;

/*
    RPL_UMODEIS
    ERR_NEEDMOREPARAMS
    ERR_UMODEUNKNOWNFLAG
    ERR_NOSUCHNICK
*/

// TODO REFACTOR setChanMode.. Its disgusting right now.. DF

// Clients class m_mode bit description
//
//  To set a bit use | x (where x is the Mask with below)
//      client->setMode( client->getMode() | 2 );
//
//  To clear a bit use & ~x (where x is the Mask with below)
//      client->setMode( client->getMode() & ~2 );
//
// Implementd     Bit   Mask with   F   Description
//      N          1        1       a - user is flagged as away;
//      Y          2        2       i - marks a users as invisible;
//      Y          3        4       w - user receives wallops;
//      Y          4        8       r - restricted user connection;
//     +Y          5        16      o - operator flag; (+ via OPER command)
//      N          6        32      O - local operator flag;
//      N          7        64      s - marks a user for receipt of server notices.
//

// @brief Mode changes the mode of a user or channel
Mode::Mode( ) : ICommand( ) {

}

Mode::~Mode( ) {

}

void Mode::cmdExecute( void ) {
    Clients* client;
    try { client = getClient(); }
    catch ( ICommandException& ex ) { return ; }

    if ( !client->isAuthenticated() || !client->isRegistered() )
        return ;

    std::vector<std::string> params;
    try { params = getParams(); }
    catch ( ICommandException& ex ) { return ; }

    if ( params.front()[0] == '#' || params.front()[0] == '&' ) {
        if ( params.front().size() <= 1 )
            return cmdError( ERR_NOSUCHCHANNEL );

        RuntimeData* rtd;
        try { rtd = getRunTimeData(); }
        catch ( ICommandException& ex ) { return ;}

        Channels* chan;
        try { chan = rtd->getChannel( params.front() ); }
        catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHCHANNEL ); }
        catch ( RuntimeData::NotFoundException& ex) { return cmdError( ERR_NOSUCHCHANNEL, params.front() ); }

        if ( params.size() == 1 )
            return sendReply( RPL_CHANNELMODEIS, "", client->getNickName(),  chan->getName() + " " +getMode( chan ) );

        if ( params.size() >= 2 )
            setChanMode( params, client, chan );
        else
            sendReply( ERR_UMODEUNKNOWNFLAG );
        return ;
    }

    // check if the client using this command is changing their own mode, otherwise reject the client
    if ( params.front() != client->getNickName() )
         return cmdError( ERR_USERSDONTMATCH , params.front() );

    if ( params.size() == 2 && ( params.front()[0] != '#' || params.front()[0] != '&' ) ) {
        setUserMode( params, client );
    }
    else if ( params.size() == 1 )
        sendReply( RPL_UMODEIS, "", client->getNickName(),  getMode( client ) );
}

// @brief static calculation to find the location of the bit
// @params[in] mode user / channel mode
// @returns[out] unsigned int, the position of the bit
// @exceptsafe no-throw
unsigned int calcBit( const unsigned int& mode ) {
    unsigned int ret;
    unsigned int cMode = mode;

    for ( ret = 0; cMode; ret++ )
        cMode = cMode >> 1;

    return ret;
}

// @brief mode sets the channel / user / channelop mode
// @params[in] client The client executing the command
// @params[in] target the Target of the command user / channel
// @params[in] channelName Channel name to apply the mode to, optional
// @params[in] mode mask of the mode ie 2,4,6...32
// @params[in] token + to add - to remove
// @exceptsafe no-throw
void Mode::mode( Clients* client, const std::string& target, const std::string& channelName, const unsigned int& mode, const char& token ) {
    if ( !client )
        return ;

    std::string cmd = "MODE";

    if ( ( token != '+' && token != '-' ) )
        return cmdError( ERR_NEEDMOREPARAMS );

    RuntimeData* rtd;
    try { rtd = getRunTimeData(); }
    catch ( ICommandException& ex ) { return ; }

    if ( !channelName.empty() && ( channelName[0] == '#' || channelName[0] == '&' ) ) {
        Channels* channel;

        try { channel = rtd->getChannel( channelName ); }
        catch ( std::invalid_argument& ex ) { return cmdError( ERR_NOSUCHCHANNEL ); }
        catch ( RuntimeData::NotFoundException& ex ) { return cmdError( ERR_NOSUCHCHANNEL, channelName ); }

        if ( target.empty() ) {
            std::string modeFlag = std::string( " beliIkmstn" );

            channel->setChanMode( token == '+' ? channel->getChanMode() | mode : ( channel->getChanMode() & ~mode ) );

            sendReply( client->getNickName()+ "!~"  + client->getUserName() + "@" + client->getHostName(), cmd, "", channel->getName() + " " + libft::to_string( token ) + modeFlag[calcBit( mode )] );
            sendReplyToList( channel->getClientList(), client->getNickName()+ "!~"  + client->getUserName() + "@" + client->getHostName(), cmd, libft::to_string( token ) + modeFlag[calcBit( mode )], channel->getName() );
        }
        else {

            Clients* fClient;
            try {
                fClient = rtd->getClientFromNick( target );
                channel->setChanOpMode( fClient, token == '+' ? channel->getChanOpMode( fClient ) | mode : channel->getChanOpMode( fClient ) & ~mode );
            }
            catch( RuntimeData::NotFoundException& ex ) { return cmdError( ERR_NOSUCHNICK, target ); }
            catch( Channels::NotFoundException& ex ) { return ; }
            catch( std::runtime_error& ex ) { return ; }

            std::string modeFlag = std::string( " vohaq" );

            sendReply( client->getNickName()+ "!~"  + client->getUserName() + "@" + client->getHostName(), cmd, "", channel->getName() + " " + libft::to_string( token ) + modeFlag[calcBit( mode )] + " " + fClient->getNickName() );
            sendReplyToList( channel->getClientList(), client->getNickName()+ "!~"  + client->getUserName() + "@" + client->getHostName(), cmd + " " + channel->getName() + " " + libft::to_string( token ) + modeFlag[calcBit( mode )] + " " + fClient->getNickName(), "", " " );
        }
    }

    if ( channelName.empty() ) {
        std::string modeFlag = std::string( " aiwroOs" );

        client->setMode( token == '+' ? client->getMode() | mode : ( client->getMode() & ~mode ) );
        sendReply( libft::to_string( SERVER ), cmd, "", client->getNickName() + " " + libft::to_string( token ) + modeFlag[calcBit( mode )] );
    }
}

// @brief sets the channel mode from cmdExecute
// @params[in] params the parameters from the cmdExecute()
// @params[in] client client executing the command
// @params[in] channel The channel object to operate on
// @exceptsafe no-throw
void Mode::setChanMode( const std::vector<std::string>& params, Clients* client, Channels* channel ) {
    unsigned char chanMode;

    try { chanMode = channel->getChanOpMode( client ); }
    catch ( Channels::NotFoundException& ex ) { return cmdError( ERR_CHANOPRIVSNEEDED, channel->getName() ); }

    if ( !( chanMode & 2 ) )
        return cmdError( ERR_CHANOPRIVSNEEDED, client->getNickName() + " " + channel->getName() );

    char token = ' ';
    size_t argCount = 2;

    for ( std::string::const_iterator it = (params[1]).begin(); it != (params[1]).end(); ++it ) {
        switch ( *it ) {
            case '+':
                token = *it;
                break;

            case '-':
                token = *it;
                break;

            case 'b':
                sendReply( RPL_ENDOFBANLIST, channel->getName() );
                break;

            case 'o':
                if ( params.size() <= argCount ) {
                    sendReply( ERR_INVALIDMODEPARAM, "Missing parameter ", channel->getName(), "+o <nick>" );
                    break;
                } else {
                    mode( client, params[argCount], channel->getName(), 2, token );
                    ++argCount;
                }
                break;

            case 'v':
                if ( params.size() <= argCount ) {
                    sendReply( ERR_INVALIDMODEPARAM, "Missing parameter ", channel->getName(), "+v <nick>" );
                    break;
                } else {
                    mode( client, params[argCount], channel->getName(), 1, token );
                    ++argCount;
                }
                break;

            case 'i':
                mode( client, "", channel->getName(), 8, token );
                break;

            case 'm':
                mode( client, "", channel->getName(), 64, token );
                break;

            case 's':
                mode( client, "", channel->getName(), 128, token );
                break;

            case 't':
                mode( client, "", channel->getName(), 256, token );
                break;

            case 'n':
                mode( client, "", channel->getName(), 512, token );
                break;

            case 'k':
                if ( params.size() <= argCount ) {
                    sendReply( ERR_INVALIDMODEPARAM, "Missing parameter ", channel->getName(), "+k <key>" );
                    break;
                }
                try { channel->setKey( params[argCount] ); }
                catch ( std::runtime_error& ex ) {
                    cmdError( ERR_INVALIDKEY, params[argCount] );
                    break;
                }
                mode( client, params[argCount], channel->getName(), 32, token );
                ++argCount;
                break;


            case 'l':
                if ( params.size() <= argCount ) {
                    sendReply( ERR_INVALIDMODEPARAM, "Missing parameter ", channel->getName(), "+l <int>" );
                    break;
                }

                try { channel->setChanLimit( libft::stol( params[argCount] ) ); }
                catch ( std::invalid_argument& ex ) {
                    sendReply( ERR_INVALIDMODEPARAM, "Parameter not a number", channel->getName(), "+" + libft::to_string( *it ) );
                    break;
                }
                mode( client, params[argCount], channel->getName(), 4, token );
                ++argCount;
                break;

            default:
                cmdError( ERR_UMODEUNKNOWNFLAG );
                break;
        }
    }
}

// @brief Set inital user mode on registration of client, only called during registration
// @params[in] params Parameters from client
// @params[in] client Client record class of client using command
//  to signal the getUserMode to send the results to the client
// @exceptsafe no-throw
void Mode::setUserMode( const std::vector<std::string>& params, Clients* client ) {
    char token = ' ';

    // Check mode flags and set
    for ( std::string::const_iterator it = params.back().begin(); it != params.back().end(); ++ it) {
        switch ( *it ) {
            case '+':
                token = *it;
                break;

            case '-':
                token = *it;
                break;

            case 'i':
                mode( client, params.front(),  "", 2, token );
                break;

            case 'w':
                mode( client, params.front(), "", 4, token );
                break;

            case 'r':
                mode( client, params.front(), "", 8, token );
                break;

            default:
                cmdError( ERR_UMODEUNKNOWNFLAG );
                break;
        }
    }
}

// @brief Reutnrs the user mode
// @params[in] client Client record class of client using command
// @returns[out] std::string, the mode ie.. +iwro
// @exceptsafe no-throw
std::string Mode::getMode( Clients* client ) {
    std::string modeFlag;

    modeFlag.append( ( client->getMode() & 2 ) ? "+i" : "-i" );
    modeFlag.append( ( client->getMode() & 4 ) ? "+w" : "-w" );
    modeFlag.append( ( client->getMode() & 8 ) ? "+r" : "-r" );
    modeFlag.append( ( client->getMode() & 16 ) ? "+o" : "-o" );

    return modeFlag;
}

// @brief Reutnrs the channel mode
// @params[in] channel Channel pointer
// @returns[out] std::string, the mode ie.. +likmstn
// @exceptsafe no-throw
std::string Mode::getMode( Channels* channel ) {
    std::string modeFlag;

    modeFlag.append( "+" );

    modeFlag.append( ( channel->getChanMode() & 4 ) ? "l" : "" );
    modeFlag.append( ( channel->getChanMode() & 8 ) ? "i" : "" );
    modeFlag.append( ( channel->getChanMode() & 32 ) ? "k" : "" );
    modeFlag.append( ( channel->getChanMode() & 64 ) ? "m" : "" );
    modeFlag.append( ( channel->getChanMode() & 128 ) ? "s" : "" );
    modeFlag.append( ( channel->getChanMode() & 256 ) ? "t" : "" );
    modeFlag.append( ( channel->getChanMode() & 512 ) ? "n" : "" );

    return modeFlag;
}
