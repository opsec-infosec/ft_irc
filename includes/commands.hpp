/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:49:50 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/09 20:35:13 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
# define COMMANDS_HPP
# include "ICommand.hpp"

// Use the following rules and you will not get in trouble
//
//  Client Message Queue
//      The developer can access the clients message queue ( m_clients->getMsgQ() ),
//      but be warned as the message queue is passed as a reference.
//      Any modifications, additions or deletions affect all other commands as
//      the message queue has all messages from the client including other commands to execute.
//      Treat the message queue as READ ONLY.  (meaning you can look, but don't touch)
//      The message queue for a specific command is automatically purged after the command has been executed
//
//  Sending Error Messages to Client
//      When sending error messages to the client, don't forget to actually send the message
//      with sendRply( ).  There are several different overloads available including one that allows
//      the developer to compose their own raw messages, but you should use the predefined ones as a priority
//
//  Help Text
//      TO BE IMPLEMENTED
//
//  Destructors
//      Please clean up after yourself, your mother is not at 42
//

using namespace irc;
// IRC Commands

// @brief Base Empty command (non searcheable, returned if no command found)
//  it basically does nothing and should remain like that.
class BaseCmd : public ICommand {
    public:
        BaseCmd( );
        ~BaseCmd( );

        void cmdExecute( void );

    private:
};

// @brief CAP command
class Cap : public ICommand {
    public:
        Cap( );
        ~Cap( );

        void cmdExecute( void );

    private:
};

//@brief JOIN command
class Join : public ICommand {
    public:
        Join( );
        ~Join( );

        void    cmdExecute( void );
        void    join( Clients* client, const std::string& chanName, const std::string& key );
};

// @brief PASS command
class Pass : public ICommand {
    public:
        Pass( );
        ~Pass( );

        void cmdExecute( void );

    private:
};

// @brief NAMES command
class Names : public ICommand {
    public:
        Names( );
        ~Names( );

        void    cmdExecute( void );
        void    names( Clients* client, const std::string& chanName );
};

// @brief NICK command
class Nick : public ICommand {
    public:
        Nick( );
        ~Nick( );

        void cmdExecute( void );

    private:
};

// @brief USER command
class User : public ICommand {
    public:
        User( );
        ~User( );

        void cmdExecute( void );

    private:
};

// @brief QUIT command
class Quit : public ICommand {
    public:
        Quit( );
        ~Quit( );

        void cmdExecute( void );

        void quit( const Clients* client, const std::string& source, const std::string& reason );
        void quit( const std::vector<Clients *> clients, const std::string& source, const std::string& reason );

    private:
};

// @brief HELP command
class Help : public ICommand {
    public:
        Help( );
        ~Help( );

        void cmdExecute( void );

    private:
};

// @brief MODE command
class Mode : public ICommand {
    public:
        Mode( );
        ~Mode( );

        void cmdExecute( void );
        void mode ( Clients* client, const std::string& target, const std::string& channelName, const unsigned int& mode, const char& token );

    private:
        std::string getMode( Clients* client );
        std::string getMode( Channels* channel );
        void setUserMode( const std::vector<std::string>& params, Clients* client );
        void setChanMode( const std::vector<std::string>& params, Clients* client, Channels* channel );
};

// @brief MODE command
class Rehash : public ICommand {
    public:
        Rehash( );
        ~Rehash( );

        void cmdExecute( void );

    private:
};

    // @brief KILL command
class Kill : public ICommand {
    public:
        Kill( );
        ~Kill( );

        void cmdExecute( void );
        std::pair<IReplies::t_reply, std::string> kill( Clients* client,  const std::string& targetNick, const std::string& reason );

    private:
};

class Kick : public ICommand {
    public:
        Kick( );
        ~Kick( );

        void    cmdExecute( void );
        void    kick( Clients* client, Channels* channel, const std::string& kickTarget, const std::string& reason );
};

// @brief PING command
class Ping : public ICommand {
    public:
        Ping( );
        ~Ping( );

        void cmdExecute( void );

    private:
};

// @brief OPER command
class Oper : public ICommand {
    public:
        Oper( );
        ~Oper( );

        void cmdExecute( void );

    private:
};

// @brief PRIVMSG command
class Privmsg : public ICommand {
    public:
        Privmsg( );
        ~Privmsg( );

        void cmdExecute( void );

        std::pair<IReplies::t_reply, std::string> privmsg( const Clients* source, const std::string& target, const std::string& message );
        std::pair<IReplies::t_reply, std::string> privmsg( const Clients* source,  const std::vector<Clients *>& targets, const std::string& message );

    private:
};

// @brief NOTICE command
class Notice : public ICommand {
    public:
        Notice( );
        ~Notice( );

        void cmdExecute( void );

        void notice( const Clients* source, const std::string& target, const std::string& message );
        void notice( const Clients* source,  const std::vector<Clients *>& targets, const std::string& message );

    private:
};

    // @brief NOTICE command
class Restart : public ICommand {
    public:
        Restart( );
        ~Restart( );

        void cmdExecute( void );
        void restart( void );

    private:
};

        // @brief NOTICE command
class Squit : public ICommand {
    public:
        Squit( );
        ~Squit( );

        void cmdExecute( void );
        void squit( const std::string& source, const std::string& comment );

    private:
};

class Wallops : public ICommand {
    public:
        Wallops( );
        ~Wallops( );

        void cmdExecute( void );
        void wallops( const std::string& source, const std::string& comment );

    private:
};

class Topic : public ICommand {
    public:
        Topic( );
        ~Topic( );

        void cmdExecute( void );
        void topic( const Clients* client,  const std::string& chanName, const std::string& topic );

    private:
};


class Part : public ICommand {
    public:
        Part( );
        ~Part( );

        void cmdExecute( void );
        void part( Clients* source, const std::string& target, const std::string& reason = "Leaving" );

    private:
};

class Invite : public ICommand {
    public:
        Invite( );
        ~Invite( );

        void cmdExecute( void );
        void invite( Clients* targetClient, Channels* channel  );
        void invite( const std::string& targetNickName, const std::string& channelName );

    private:

};

class Who : public ICommand {
    public:
        Who( );
        ~Who( );

        void cmdExecute( void );
        void who( const Clients* client, const std::string& mask );

    private:
        std::string chOp( const unsigned char& chOpMode ) const;

};
#endif
