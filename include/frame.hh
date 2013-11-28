#ifndef FRAME_HH_
# define FRAME_HH_

# include 	<string>
# include 	<map>
# include 	"filter.hh"
# include 	"frameImage.hh"

class 			Frame
{
	protected:
		std::map<const std::string &, Filter *> *_filter;
	public:
		Frame();
		virtual ~Frame();
		void 								addFilter(const std::string &, Filter *);
		void 								removeFilter(const std::string &);
		virtual void 				*getImage() = 0;
};

#endif
