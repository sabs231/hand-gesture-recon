#ifndef FRAME_HH_
# define FRAME_HH_

# include 	<map>
# include 	<string>
# include 	"filter.hh"
# include 	"frameImage.hh"

class 			Filter;

class 			Frame
{
	protected:
		std::map<std::string, Filter *> *_filter;
	public:
		Frame();
		virtual ~Frame();
		void 								addFilter(std::string, Filter *);
		void 								removeFilter(std::string);
		virtual void 				*getImage() = 0;
		virtual void 				setImage(void *) = 0;
		virtual void 				showImage(const std::string &) = 0;
};

#endif
