SRC_EXCEP		=		src/exception/exception.cpp

SEOBJ			=		$(SRC_EXCEP:.cpp=.o)

SRC				=		src/main.cpp

SOBJ			=		$(SRC:.cpp=.o)

NAME			=		bin/grecon

CXX				=		g++

CXXFLAGS		+=		-I./include

CXXFLAGS		+=		-W -Wall -Wextra -Werror

OPENCVLIB		=		-lopencv_core -lopencv_flann -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_objdetect -lopencv_photo -lopencv_nonfree -lopencv_features2d -lopencv_calib3d -lopencv_legacy -lopencv_contrib -lopencv_stitching -lopencv_videostab -lopencv_gpu -lopencv_ts

RM				=		rm -f

ECHO			=		echo

%.o:			%.cpp
				$(CXX) -c -o $@ $< $(CXXFLAGS)

all:			$(NAME)

$(NAME):		$(SEOBJ) $(SOBJ)
				$(CXX) $(SEOBJ) $(SOBJ) -o $(NAME) $(OPENCVLIB)
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
