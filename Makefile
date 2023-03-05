SRCS = main.cpp Webserver.class.cpp Request.class.cpp Client.class.cpp \
	Block.tmp.cpp Response.class.cpp ErrorString.class.cpp

NAME = main

CPPFLAGS = -std=c++98 -fsanitize=address

all : $(NAME)

$(NAME) : $(SRCS)

clean :
	rm -f main

fclean : clean

re : fclean all