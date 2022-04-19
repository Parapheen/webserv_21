COLOR_RED		= 		"\033[0;31m"
COLOR_GREEN		= 		"\033[0;32m"
COLOR_RESET		= 		"\033[0m"

NAME			=		webserv

SRCS 			=		srcs/main.cpp srcs/server/TCPServer.cpp srcs/server/ClientsManager.cpp \
						srcs/server/ServersManager.cpp srcs/server/Client.cpp \
						srcs/server/Webserver.cpp srcs/server/EventLoop.cpp

OBJS			=		${SRCS:.cpp=.o}

CLANG 			=		clang++

CPPFLAGS		=		-Wall -Wextra -Werror -MMD -std=c++98

CLEAN			=		rm -f

all:		${NAME}

${NAME}:	${OBJS}
			@echo ${COLOR_GREEN}Creating ${NAME}...${COLOR_RESET}
			@${CLANG} -o ${NAME} ${OBJS}

-include	${SRCS:.cpp=.d}

clean:
			@echo ${COLOR_RED}Cleaning...${COLOR_RESET}
			${RM} ${OBJS} ${SRCS:.cpp=.d}

fclean:		clean
			${CLEAN} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re
