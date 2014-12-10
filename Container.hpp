/** Container Class
 ** Standardizes all inserts, pops, updates, etc for a container so that these functions are independent of type
 **/

using namespace std;

template <typename V>
class Container{
 private:

 public:

	/** Element type. */
	typedef typename V::value_type value_type;
	/** Type of pointers to elements. */
	typedef typename V::pointer pointer;
	/** Type of references to elements. */
	typedef typename V::reference reference;
	/** Iterator category. */
	typedef std::input_iterator_tag iterator_category;
	/** Difference between iterators */
	typedef typename V::difference_type difference_type;
	/** Size type */
	typedef typename V::size_type size_type;
	/** Iterator type */
	typedef typename V::iterator iterator;
	/** Key type */
	typedef typename V::key_type key_type;

	Container(){}
	
	iterator begin(){
		return v_.begin();
	}

	iterator end(){
		return v_.end();
	}

	reference front(){
		return *begin();
	}

	reference back(){
		if (empty())
			return *(back());
		else
			return *(begin()+(size()-1));
	}

	value_type pop(){
		value_type a = front();
		v_.erase(begin());
		return a;
	}

	value_type push(const value_type& a){
		return v_.insert(a);
	}

	void clear(){
		v_.clear();
	}

	size_type size(){
		return v_.size();
	}

	void erase(iterator it){
		v_.erase(it);
	}

	void erase(key_type& k){
		v_.erase(k);
	}

	reference operator[](key_type& k){
		return *(v_.find(k));
	}

	reference operator[](size_type k){
		return *(begin() + k);
	}

	bool empty(){
		return v_.size()==0;
	}

	V& operator=(const V& v){
		clear();
		v_.insert(v.begin(),v.end());
		return v_;
	}
  private:

		V v_;
};







