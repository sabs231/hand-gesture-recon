#ifndef RELEVANCEVECTOR_HPP_
# define RELEVANCEVECTOR_HPP_

template 	<typename T>
class			RelevanceVector
{
	private:
		T 											*_relevanceV;

	public:
		RelevanceVector(unsigned int size)
		{
			this->_relevanceV = new T[size];
		} 

		~RelevanceVector()
		{
			if (this->_relevanceV)
				delete this->_relevanceV;
		}

		T 		*getRelevanceVector()
		{
			return (this->_relevanceV);
		}

		void 	setRelevanceVector(T *vector)
		{
			this->_relevanceV = vector;
		}
};

#endif
