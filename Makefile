CC = clang++
FLAGS = -std=c++98 -Wall -Wextra -Werror -g -MMD

OBJ = $(SRCS:.cpp=.o)
RM = rm -f
NAME = webserver

SRCS 			=		srcs/main.cpp srcs/server/TCPServer.cpp srcs/server/ClientsManager.cpp \
						srcs/server/ServersManager.cpp srcs/server/Client.cpp \
						srcs/server/Webserver.cpp srcs/server/EventLoop.cpp \
						srcs/parser/Lexer.cpp srcs/parser/Parser.cpp srcs/parser/ServerCfg.cpp \
						srcs/parser/LocationCfg.cpp srcs/http/Request.cpp srcs/http/Response.cpp \
						srcs/cgi/CgiHandler.cpp

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)

-include $(SRCS:.cpp=.d)

all: $(NAME)

clean:
	$(RM) $(OBJ) $(SRCS:.cpp=.d)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean fclean all re