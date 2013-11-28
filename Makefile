SRC_EXCEP		=		src/exception/exception.cpp

SEOBJ			=		$(SRC_EXCEP:.cpp=.o)

SRC_FIL			=		src/filtering/filterGrayScaleOPCV.cpp

SFOBJ			=		$(SRC_FIL:.cpp=.o)

SRC_IO			=		src/io/frame.cpp		\
						src/io/frameOPCV.cpp	\
						src/io/inputOPCV.cpp

SIOBJ			=		$(SRC_IO:.cpp=.o)

SRC_MD			=		src/motiondetection/environmentOPCV.cpp			\
						src/motiondetection/mhiOPCV.cpp					\
						src/motiondetection/motionDetect.cpp			\
						src/motiondetection/relevanceVectorOPCV.cpp		

SMDOBJ			=		$(SRC_MD:.cpp=.o)

SRC_ML			=		src/machinelearning/NBayesClassifierOPCV.cpp

SMLOBJ			=		$(SRC_ML:.cpp=.o)

SRC				=		src/main.cpp

SOBJ			=		$(SRC:.cpp=.o)

NAME			=		bin/grecon

CXX				=		g++

CXXFLAGS		+=		-I./include

CXXFLAGS		+=		-W -Wall -Wextra -Werror

OPENCVLIB		=		-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_features2d -lopencv_ml

RM				=		rm -f

ECHO			=		echo

%.o:			%.cpp
				$(CXX) -c -o $@ $< $(CXXFLAGS)

all:			$(NAME)

$(NAME):		$(SEOBJ) $(SIOBJ) $(SFOBJ) $(SMDOBJ) $(SMLOBJ) $(SOBJ)
				$(CXX) $(SEOBJ) $(SIOBJ) $(SFOBJ) $(SMDOBJ) $(SMLOBJ) $(SOBJ) $(OPENCVLIB) -o $(NAME)
				@$(ECHO) '---> grecon compiled'

clean:
				$(RM) $(SEOBJ)
				$(RM) $(SOBJ)
				$(RM) $(SIOBJ) 
				$(RM) $(SFOBJ)
				$(RM) $(SMDOBJ)
				$(RM) $(SMLOBJ)
				@$(ECHO) '---> directory cleaned'

fclean:			clean
				$(RM) $(NAME)
				@$(ECHO) '---> removed excutables'

re:				fclean all

.PHONY:			all clean fclean re
