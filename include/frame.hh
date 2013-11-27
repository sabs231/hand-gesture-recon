#ifndef FRAME_HH_
# define FRAME_HH_

# include 	<vector>
# include 	"filter.hh"
# include 	"frameImage.hh"

class 			Frame
{
	protected:
		std::vector<Filter *> *_filter;
	public:
		Frame();
		virtual ~Frame();
		void 								addFilter(Filter *);
		void 								removeFilter(Filter *);
		virtual void 				*getImage() = 0;
};

#endif
