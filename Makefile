SRCS = main.cpp Webserver.class.cpp Request.class.cpp Client.class.cpp

NAME = main

# CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address

all : $(NAME)

$(NAME) : $(SRCS)

clean :
	rm -f main

fclean : clean

re : fclean all