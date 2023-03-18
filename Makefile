SRCS = main.cpp Webserver.class.cpp Request.class.cpp Client.class.cpp \
	Block.tmp.cpp Response.class.cpp ErrorString.class.cpp MimeTypes.class.cpp \
	BodyParser.class.cpp ParseConfigfile/Configfile.cpp Serverblock.cpp location.cpp

HEADER = ParseConfigfile/Configfile.hpp Serverblock.hpp

NAME = main

CPPFLAGS = -std=c++98 -fsanitize=address

all : $(NAME)

$(NAME) : $(SRCS) $(HEADER)

clean :
	rm -f main

fclean : clean

re : fclean all

run : re
	clear && ./main