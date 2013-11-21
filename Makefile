SRC_EXCEP		=		src/exception/exception.cpp

SEOBJ			=		$(SRC_EXCEP:.cpp=.o)

SRC				=		src/main.cpp

SOBJ			=		$(SRC:.cpp=.o)

NAME			=		bin/grecon

CXX				=		clang++

CXXFLAGS		+=		-I./include

CXXFLAGS		+=		-W -Wall -Wextra -Werror

OPENCVLIB		=		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_features2d

RM				=		rm -f

ECHO			=		echo

%.o:			%.cpp
				$(CXX) -c -o $@ $< $(CXXFLAGS)

all:			$(NAME)

$(NAME):		$(SEOBJ) $(SOBJ)
				$(CXX) $(SEOBJ) $(SOBJ) $(OPENCVLIB) -o $(NAME)
				@$(ECHO) '---> grecon compiled'

clean:
				$(RM) $(SEOBJ)
				$(RM) $(SOBJ)
				@$(ECHO) '---> directory cleaned'

fclean:			clean
				$(RM) $(NAME)
				@$(ECHO) '---> removed excutables'

re:				fclean all

.PHONY:			all clean fclean re
