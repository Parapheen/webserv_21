CC = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror
SRCS = webserver.cpp

OBJ = $(SRCS:.cpp=.o)
RM = rm -f
NAME = webserver

SRCS 			=		srcs/main.cpp srcs/server/TCPServer.cpp srcs/server/ClientsManager.cpp \
						srcs/server/ServersManager.cpp srcs/server/Client.cpp \
						srcs/server/Webserver.cpp srcs/server/EventLoop.cpp \
						srcs/parser/Lexer.cpp srcs/parser/Parser.cpp srcs/parser/ServerCfg.cpp \
						srcs/parser/LocationCfg.cpp

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ)

all: $(NAME)

clean:
	$(RM) $(OBJ) $(OBJ_FT) $(OBJ_STD)

fclean:
	$(RM) $(NAME) $(NAME_FT) $(NAME_STD) $(OBJ) $(OBJ_FT) $(OBJ_STD)

re: fclean all

ft: $(NAME_FT)

std: $(NAME_STD)


