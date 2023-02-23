NAME  = main

SRC = ParseConfigfile/Configfile.cpp Serverblock.cpp location.cpp main.cpp 

HEADER = ParseConfigfile/Configfile.hpp Serverblock.hpp

CC = c++

all : $(NAME)


$(NAME) : $(SRC) $(HEADER)
	$(CC) -fsanitize=address $(SRC)  -o $(NAME)

clean : 
	rm -rf $(NAME)

fclean : clean
re : clean all