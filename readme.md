
```
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dfurneau <dfurneau@student.42abudhabi.ae>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/07 08:12:05 by dfurneau          #+#    #+#             */
/*   Updated: 2023/03/07 08:12:05 by dfurneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hawadh <hawadh@student.42Abudhabi.ae>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/24 20:35:31 by hawadh            #+#    #+#             */
/*   Updated: 2022/03/24 18:39:06 by hawadh           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

```

# FT_IRC

## IRC Server

Our ft_irc server project written in C++.  The project required the students to create an IRC (Internet Relay Chat) server using poll and to funciton

similar to a real ft_irc without server to server communications using a single even driven thread.

<br>

IRC is based on an open TCP protocol developed in 1988 by Jarkko Oikarinen at the University of Oulu in Finland.  The IRC server is based on a

 request / response model, where clients issue commands to the server and the server responds to the client with a reply.

 <br>

 THe poll method allows for multiple clients without the need for threading as the communications are multiplexed over one socket connection.

 The server blocks on the poll method until an even is received, either POLLIN (receive) or POLLOUT (send), and then processes

the events.  Receive messages are placed into a temporary buffer in order to capture a full message, once a full message is received, the message is split

into the respective parameters and placed into an internal queue, deque of string vectors.  The server then processes the command send by the client and

executes the command through a commandWrapper method that does the lookup for the command and returns a pointer to the coresponding command method.

All command methods use the ICommand.cpp interface (Abstract Base Class) to implement all the underlying low level functions required for each command.

Each command uses the ICommand interface to provide the command, paramaters and any trailing messages, process the command and any logic required, and

then send a reply into the send queue using the built-in functions available in the interface.  This architecture makes it very easy to make changes to

the underlying low level functions without modifying each and every commands code base.  The code base is isolated between the server and the commands, with the

interface the only method of accessing data from the channels, other clients and the server.  Once the reply is placed into the send queue, the command returns to

main server loop, where poll with unblock and fire a POLLOUT event.  The POLLOUT event then sends the message reply queue back to the client.  Each client has their

own receive / send message queues as described above.

<br>

In addition to the IRC server, there is a custom logger that captures logs to a file (and example is 20230412.log) to aid in understanding and debuging the server.

And finally there is Marvin, Marvin is a bot that can give you channel ops if there are no channel ops in a channel.

By messaging Marvin (PRIVMSG Marvin :Help), Marvin wil hop into your channel, give himself ChannelOps permission, and then give the client requesting help ChannelOps.

After thanking Marvin (PRIVMSG \<channel\> :Thanks), Marvin departs the channel.  Marvin connects to the IRC server much like a client, using TCP sockets send / receive

messages to interface with the IRC server.  In addition, sinc Marvin is essentially living inside the computer, he has access to all the data structures of the server

code base and can manipulate the structures to provide the access a client needs.  On each server event (Send / Receive) Marvins code is executed and processed to

parse the messages it receives.  Thanks to Hussain for helping out with the project, he is a great asset to have on any project and is eager to learn and come up with

new ways to implement ideas into the code base, I learned some new concepts from him and I enjoyed every minute of it!

<br>

## Load Testing

The client used to test was irssi v1.2.3.  The server was tested with ft_irc-tester and handled around 500+ client connections into a single channel and their respective

channel, (without PRIVMSG to the channel, as this causes a flooding type scenario) and about 100+ clients into a single channel with each client randomly sending messages to the

same channel.

<br>

## Commands

The following commands were implemented in this IRC server:

* CAP
* INVITE
* JOIN
* PART
* KICK
* KILL
* MODE
* NAMES
* NICK
* NOTICE
* OPER
* PASS
* PING
* PRIVMSG
* QUIT
* REHASH
* RESTART
* SQUIT
* TOPIC
* USER
* WALLOPS
* WHO
<br>
<br>

## `DOCS`

IRC Reference website
[Modern IRC Client Protocol](https://modern.ircdocs.horse)

Original RFC
[RFC1459](https://www.rfc-editor.org/rfc/rfc1459)

RFC IRC Architecture
[RFC2810](https://www.rfc-editor.org/rfc/rfc2810)

RFC for Servers handling of Channels
[RFC2811](https://www.rfc-editor.org/rfc/rfc2811)

RFC for Clients
[RFC2812](https://www.rfc-editor.org/rfc/rfc2812)

RFC for Server to Server
[RFC2813](https://www.rfc-editor.org/rfc/rfc2813)

RFC for SSL/TLS Connections
[RFC7194](https://www.rfc-editor.org/rfc/rfc7194)

<br>

## `REFERENCE CLIENT`

[Client ircss](https://irssi.org/)

[Client ircss Source Code](https://github.com/irssi/irssi)

***NOTE: Version 1.2.3 is used as the reference***
