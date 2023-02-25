SRCS = main.cpp Webserver.class.cpp Request.class.cpp

NAME = main

# CPPFLAGS = -Wall -Wextra -Werror -std=c++98

all : $(NAME)

$(NAME) : $(SRCS)

clean :
	rm -f main

fclean : clean

re : fclean all