/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ICommand.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 05:50:00 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 22:35:08 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ICOMMAND_HPP
# define ICOMMAND_HPP
# include "clients.hpp"
# include "runtime.hpp"
# include "IReplies.hpp"

# include <algorithm>
# include <string>
# include <vector>
# include <deque>
# include <map>
# include <stdexcept>
# include <unistd.h>

namespace irc {
    //@brief Base Command ABC, inherit from this base class to implement commands
    class ICommand : protected IReplies {
        public:
            ICommand( );

            virtual ~ICommand( ) = 0;

            virtual void cmdExecute( void ) = 0;
            virtual void cmdError( const IReplies::t_reply& errNumber, const std::string& data = std::string() );

            class ICommandException : public std::runtime_error {
                public:
                    ICommandException( const std::string& ex ) : std::runtime_error( ex ) {
                        std::cout << this << " ICommand " << this->what() << std::endl;
                    }
            };

            void setClient( Clients* client );
            void clearClient( void );
            void loadRunTimeData( RuntimeData* rtd );

        protected:
            void sendReply( const std::string& rply );
            void sendReply( const std::string& source, const std::string& command, const std::string& trailing = "", const std::string& target = "" );
            void sendReply( const IReplies::t_reply& errNumber,  const std::string& trailing = "", const std::string& target = "", const std::string& operations = "" );

            void sendReplyTo( const std::string& nickName, const std::string& rply );
            void sendReplyTo( const std::string& nickName, const std::string& source, const std::string& command, const std::string& trailing = "", const std::string& target = "" );

            void sendReplyToList( const std::vector<Clients *>& clientLIst,
                const IReplies::t_reply& errNumber,
                const std::string& trailing = "",
                const std::string& target = "",
                const std::string& operations = "",
                const std::string& source = ""
            );
            void sendReplyToList( const std::vector<Clients *>& clientList,
                const std::string& source,
                const std::string& command,
                const std::string& trailing = "",
                const std::string& target = ""
            );
            void sendReplyToList( const std::vector<Clients *>& clientList, const Clients* client,  const std::string& errNumber, const std::string& reply );


            void close( const int& fd );

            std::string getCmd( void );
            const std::vector<std::string>& getParams( void );
            const std::string& getTrailing( void );
            Clients* getClient( void );
            RuntimeData* getRunTimeData( void );

        private:
            Clients* m_client;
            RuntimeData* m_rtd;
            std::string m_command;
            std::vector<std::string> m_params;
            std::string m_trailing;



    };
}
#endif
