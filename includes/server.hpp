/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/14 10:10:39 by dfurneau          #+#    #+#             */
/*   Updated: 2023/04/12 22:57:56 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# ifndef BOT
#  define BOT false
# endif

# include "clients.hpp"
# include "commandWrapper.hpp"
# include "runtime.hpp"
# include "libft.hpp"
# include "logger.hpp"
# include "channels.hpp"
# include "marvin.hpp"
# include "serverData.hpp"
# include <csignal>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <stdexcept>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>
# include <poll.h>
# include <fcntl.h>
# include <unistd.h>

namespace irc {
    class Server {
        public:
            // Constructor Destructor
            Server( );
            Server( const std::string& host, const int& port, const std::string& password );
            ~Server( );

            // Public Vars
            typedef enum t_status { OFFLINE, ONLINE } e_status;

            // Public Methods
            void init( void );
            void start( void );
            void stop( void );

            // Public Getters
            std::vector<Clients *>& getClients( void );

            // Public Setters
            void setPFD( int fd  );

        private:
            // Private Vars
            const std::string m_host;
            const int m_port;
            int m_socket;
            ServerData* m_sData;
            e_status m_status;
            std::vector<Clients *> m_clients;
            std::vector<Channels *> m_channels;
            std::vector<pollfd> m_pFD;
            Marvin* m_marv;
            std::string m_startUpTime;
            commandWrapper* m_clientCommand;
            Logger m_log;

            // Private Methods
            void removeClient( int fd );
            void removeChannel( void );
            bool isOnline( void );
            static void sigInit( void );
            std::string printMsgQ( std::deque<std::vector<std::string> > msgQ ) const;
    };
}

#endif
