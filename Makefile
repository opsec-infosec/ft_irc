# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/07 07:25:25 by dfurneau          #+#    #+#              #
#    Updated: 2023/04/13 02:40:04 by dfurneau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

##### Logger Functionality ######

LOG		?= false
GMT		?= false

#################################

######## Bot Functionalit #######

BOT		?= false

#################################



NAME		=	ircserv

SSRCS		=	./src/main.cpp \
				./src/libft.cpp \
				./src/logger.cpp \
				./src/marvin.cpp \
				./src/server.cpp \
				./src/clients.cpp \
				./src/runtime.cpp \
				./src/ICommand.cpp \
				./src/channels.cpp \
				./src/commands/cap.cpp \
				./src/commands/who.cpp \
				./src/commands/pass.cpp \
				./src/commands/nick.cpp \
				./src/commands/user.cpp \
				./src/commands/mode.cpp \
				./src/commands/help.cpp \
				./src/commands/ping.cpp \
				./src/commands/kill.cpp \
				./src/commands/kick.cpp \
				./src/commands/quit.cpp \
				./src/commands/oper.cpp \
				./src/commands/join.cpp \
				./src/commands/part.cpp \
				./src/commands/names.cpp \
				./src/commands/topic.cpp \
				./src/commandWrapper.cpp \
				./src/commands/squit.cpp \
				./src/commands/notice.cpp \
				./src/commands/rehash.cpp \
				./src/commands/invite.cpp \
				./src/commands/privmsg.cpp \
				./src/commands/restart.cpp \
				./src/commands/wallops.cpp \
				./src/commands/baseCmd.cpp \


SOBJS		=	${SSRCS:.cpp=.o}
CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -D LOG=${LOG} -D GMT=${GMT} -D BOT=${BOT}
DEBUG		=	-g3
OPTIMIZE	=	-O2

RM			=	rm -f

all:		${NAME}

bonus:
	 		make all BOT=true

${NAME}:	${SOBJS}
			${CXX} ${CXXFLAGS} ${SOBJS} -o ${NAME} -I ./includes/

clean:

			${RM} ${NAME}

fclean:		clean

			${RM} ${SOBJS}

re:			fclean all

.PHONY : all clean re fclean
