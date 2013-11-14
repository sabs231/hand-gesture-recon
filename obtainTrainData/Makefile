
SRC_RELEVANCE	=	obtainTrainData.cpp
CXX		=	g++
CFLAGS		=	-W -Wall -Werror
#LDLIBS		=	-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_features2d
LDLIBS		=	 -lopencv_core -lopencv_flann -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_objdetect -lopencv_photo -lopencv_nonfree -lopencv_features2d -lopencv_calib3d -lopencv_legacy -lopencv_contrib -lopencv_stitching -lopencv_videostab -lopencv_gpu -lopencv_ts	
LDFLAGS		=	$(LDLIBS)
NAME_RELEVANCE	=	obtainTrainData
OBJS_RELEVANCE	=	$(SRC_RELEVANCE:.cpp=.o)
RM		=	rm -f

all:			$(NAME_RELEVANCE)

$(NAME_RELEVANCE):	$(OBJS_RELEVANCE)
				$(CXX) $(CFLAGS) $(OBJS_RELEVANCE) $(LDFLAGS) -o $(NAME_RELEVANCE)

clean:			
				$(RM) $(OBJS_RELEVANCE)

fclean:			clean
				$(RM) $(NAME_RELEVANCE)

re:			clean all

.PHONY:			all clean fclean re
