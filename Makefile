NAME = webserv

SRCS = $(addprefix srcs/, main.cpp Webserver.class.cpp Request.class.cpp Client.class.cpp \
	Response.class.cpp ErrorString.class.cpp MimeTypes.class.cpp \
	BodyParser.class.cpp Configfile.cpp Serverblock.cpp \
	location.cpp Utils.class.cpp)

HEADERS = $(addprefix includes/, Configfile.hpp Serverblock.hpp BodyParser.class.hpp \
	Client.class.hpp ErrorString.class.hpp Exception.hpp location.hpp \
	MainHeader.hpp MimeTypes.class.hpp Request.class.hpp Response.class.hpp \
	Utils.class.hpp Webserver.class.hpp)

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror

CC = c++

OBJS = $(SRCS:.cpp=.o)

%.o : %.cpp
	@echo "\033[33;3mCompiling $<\033[0m"
	@$(CC) -c $(CPPFLAGS) $< -o $@

all : $(NAME)

$(NAME) : $(OBJS) $(HEADERS)
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[32;5m------Webserv Created Successfully------\033[0m"

clean :
	@rm -f $(OBJS)
	@echo "\033[31;6m-----Object Files Deleted Successfully-----\033[0m"

fclean : clean
	@rm -f $(NAME)
	@echo "\033[31;6m-----Executable Deleted Successfully-----\033[0m"

re : fclean all

.PHONY : all clean fclean re