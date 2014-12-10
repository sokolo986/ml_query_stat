/** Sampler Class
 ** Collected & maintains samples IAW a registered policy
 ** Samples are associated with a policy; therefore, policies
 ** must be allocated for and created before samples are requested
 ** sample_value_type must define a 'collect' and a 'stopcollect' routine
 ** that can be stopped. 'collect' must be implemented as a passive function that collects samples and thenn returns to the calling function to test if the samples meet the policy
 ** must also have implemented has_met_limit()
 **/
		
template <typename P, typename S, typename C>
class Sampler{
 private:
	struct edge_null {
		unsigned edge;
	};
	typedef edge_null E;

	/**Holds policy and sample information**/
	struct policy_info_type;
	struct sample_info_type;
	typedef sample_info_type I;
	typedef Graph<I,E> GraphType;

 public:
	/** Value types **/
	typedef P policy_value_type;
	typedef S sample_value_type;
	typedef C collect_return_type;
	
	/** Graph typedefs **/
	class Policy;
	class policy_iterator;
	typedef Policy policy_type;
	typedef typename GraphType::size_type size_type;
	typedef typename GraphType::Node sample_type;
	typedef typename GraphType::Node Sample;
	typedef typename GraphType::node_value_type sample_info_type;
	typedef typename GraphType::node_iterator sample_iterator;
	typedef std::chrono::high_resolution_clock clock;

	/** Constructor for Sampler Class
	* @pre	None
 	* @param[in] max_policies  i in [1,UCHAR_MAX)
   * @post 	num_active_policies() = 0
	* @post	num_inactive_policies() = 0
	* @post	num_policies() = 0
	*/
	Sampler(size_type max_policies){
		assert((max_policies >= 1) && (max_policies<UCHAR_MAX));

		//create pool of policy ids as appropriate  
		for (size_type i=0; i<max_policies; ++i)
			free_id_set_.insert(i);

		for (size_type i=0; i<max_policies; ++i)
			policies_[i] = policy_info_type();

		for (size_type i=0; i<max_policies; ++i)
			policy2samples_[i] = GraphType();

		num_inactive_policies_ = 0;
		num_active_policies_ = 0;
		max_policies_ = max_policies;

		assert(policy2samples_.size()==policies_.size());
		assert(num_active_policies()==0);
		assert(num_inactive_policies()==0);
		assert(num_policies()==0);
	}

	/** Use Default destructor **/
	~Sampler() = default;

	/** Starts the collection of all samples
	* @post			num_active_policies()==0;
	*/
	void start_collections() {
		for(auto it = policies(); it != policies_end(); ++it)
			(*it).start_collection();
		assert (num_active_policies()==num_policies());
	}

	/** Stops the collection of all samples
	* @post			num_active_policies()==0;
	* @post			num_inactive_policies==num_policies();
	*/
	void stop_collections() {
		for(auto it = policies(); it != policies_end(); ++it)
			(*it).stop_collection();
		assert (num_active_policies()==0);
		assert (num_inactive_policies()==num_policies());
	}

	/** Increases max_policy() limit
	* @pre			max_policies()+num_policies_to_add < UCHAR_MAX
   * @post 			max_policies()+=num_policies_to_add
	*/
	void increase_num_policies(size_type num_policies_to_add){
		assert((max_num_policies()+num_policies_to_add)< UCHAR_MAX);
		size_type cur_max = max_num_policies();
		size_type max_limit = cur_max + num_policies_to_add;

		for(size_type i=cur_max; i<max_limit; ++i)
			free_id_set_.insert(i);

		for (size_type i=cur_max; i<max_limit; ++i)
			policies_[i] = policy_info_type();

		for (size_type i=cur_max; i<max_limit; ++i)
			policy2samples_[i] = GraphType();

		max_policies_ += num_policies_to_add; 
	}

	/** Creates a new policy
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/
  	Policy create_policy(size_type max_samples = 1000, size_type max_sample_size = 1000*sizeof(sample_value_type)){
		assert(num_policies()<=max_num_policies());		
		auto it = free_id_set_.begin();
		size_type pid = *it;
		free_id_set_.erase(it);
		used_id_set_.insert(pid);
		++num_inactive_policies_;
		policies_[pid].max_num_samples_ 	= max_samples;
		policies_[pid].max_size_			= max_sample_size;
		policy2samples_[pid] = GraphType();
		assert(policy2samples_.size()==policies_.size());
		return Policy(this,pid);
		
 	} 

	/** Creates a new policy copying the samples from an existing policy
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/
  	Policy create_policy(Policy p){
		assert(num_policies()<=max_num_policies());		
		auto it = free_id_set_.begin();
		size_type pid = *it;
		free_id_set_.erase(it);
		used_id_set_.insert(pid);

		++num_inactive_policies_;
		policies_[pid].value_ = p.value();
		policy2samples_[pid] = GraphType();
		assert(policy2samples_.size()==policies_.size());
		return Policy(this,pid);
 	} 

	/** Deletes a policy. Invalidates all existing pointers
	* @post 			invalidated all existing pointers/references to outstanding policies
	*/
	void delete_policy(Policy& p){
		if (p.is_active())
			--num_active_policies_;
		else
			--num_inactive_policies_;
		policy2samples_[p.get_id()].clear();
		policies_[p.get_id()] = policy_value_type();
		used_id_set_.erase(p.get_id);
		free_id_set_.insert(p.get_id);
	}

	/**Returns an iterator to the start of all policies**/
	policy_iterator policies(){
		return policy_iterator(this, 0);
	}
	/**Returns an iterator to the end of all policies**/
	policy_iterator policies_end(){
		return policy_iterator(this, num_policies());
	}

	/**Returns the policy referenced by an iterator**/
	Policy policy(policy_iterator it){
		return *it;
	}

	/**Returns max number of policies allowed */
	size_type max_num_policies() const{
		return max_policies_; 
	}

	/**Total active/inactive policies**/
	size_type num_policies() {
		assert(used_id_set_.size()==(num_active_policies_+num_inactive_policies_));
		return used_id_set_.size();
	}

	/**Returns the number of policy that are actively collecting samples**/
	size_type num_active_policies(){
		return num_active_policies_;
  	}

	/**Returns the number of policy that are not actively collecting samples**/
	size_type num_inactive_policies() {
		return num_inactive_policies_;
	}

  /**Erases all policies and samples**/
  void clear(){
		used_id_set_.clear();
		free_id_set_.clear();
		policies_.clear();
		policy2samples_.clear();

		//create pool of policy ids as appropriate  
		for (size_type i=0; i<max_policies_; ++i)
			free_id_set_.insert(i);

		for (size_type i=0; i<max_policies_; ++i)
			policies_[i] = policy_info_type();

		for (size_type i=0; i<max_policies_; ++i)
			policy2samples_[i] = GraphType();

		num_inactive_policies_ = 0;
		num_active_policies_ = 0;
		assert(policy2samples_.size()==policies_.size());
  }


  class Policy:private totally_ordered<Policy> {
	public:
   Policy(){
   }

	/**Starts collection of samples*/
	void start_collection() {
		if (is_active()) return;
		if (has_met_limit()) return;
		status() = 1;
		start_t() = clock(); 
		-- set_->num_inactive_policies_;
		++ set_->num_active_policies_;
		assert ((set_->num_active_policies_+set_->num_inactive_policies_)==set_->used_id_set_.size());
		while (!has_met_limit()){
			fetch().value_.collect();
		}
	}

	/**Stops collection of samples*/
	void stop_collection(){
		if (!is_active()) return;
		end_t() = clock();

		status() = 0;
		++ set_->num_inactive_policies_;
		-- set_->num_active_policies_;

		//clean up and remove extra number of samples
		while (num_samples() > max_samples()){
			auto it = samples_begin();
			fetch_samples().remove_node(*it);
		}

		//clean up and remove extra samples that do not meet size requirements
		size_type sizeof_sample = sizeof(sample_value_type);
		while (max_size() < (sizeof_sample * num_samples())){
			auto it = samples_begin();
			fetch_samples().remove_node(*it);
		}
	}

	policy_value_type& value(){
		return fetch().value_;
   }

	bool& status() const{
		return fetch().status_;
   }

	clock& start_t() const{
		return fetch().start_t_;
	}

	clock& end_t() const{
		return fetch().end_t_;
	}

   bool is_active() const{
		auto a =  fetch().status_;
		return a;
   }

	bool has_met_limit() {
		return ((fetch().value_.has_met_limit()) || ((max_samples() <= num_samples()) && (max_size() <= sizeof(sample_value_type)*num_samples()))); 
	}

	size_type& max_size(){
		return fetch().max_size_;
	}

   void clear(){
		stop_collection();
		fetch_samples().clear();
   }

	bool operator==(Policy& a){
		return a.uid_==(uid_);
	}

	bool operator<(Policy& a){
		return a.uid_<(uid_);
	}

   //Samples ------------------------------
	Sampler::Sample add_sample(Sampler::sample_value_type& sv){
		sample_info_type si{true,sv};
		return fetch_samples().add_node(Point(),si);
	}

	void delete_samples(){
		set_->policy2samples_[uid_].clear();
	}

   sample_iterator samples_begin(){
		return fetch_samples().node_begin();
   }

	sample_iterator samples_end(){
		return fetch_samples().node_end();
	}

	Sampler::Sample sample(Sampler::sample_iterator s){
		return (*s);
	}

	sample_value_type& sample_value(Sampler::sample_iterator s){
		return s.value(); 
   }

	size_type& max_samples(){
		return fetch().max_num_samples_;
   }

	size_type num_samples() const{
		return set_->policy2samples_[uid_].size();
	}

   private:
		friend class Sampler;
		friend class policy_iterator;

		Sampler* set_;
		size_type uid_;

		Policy(const Sampler* set, size_type uid)
		  :set_(const_cast<Sampler*>(set)),uid_(uid){
		}

		policy_info_type& fetch(){
			return set_->policies_[uid_];
		}

		GraphType& fetch_samples(){
			return set_->policy2samples_[uid_];
		}

		bool& status(){
			return fetch().status_;
   	}

		clock& start_t() {
			return fetch().start_t_;
		}

		clock& end_t() {
			return fetch().end_t_;
		}

		bool is_active() {
			return fetch().status_;
		}

  };

	
  /** @class Sampler::policy_iterator
   * @brief Iterator class for policies. A forward iterator. */
  class policy_iterator: private totally_ordered<policy_iterator> {
   public:
    // These type definitions help us use STL's iterator_traits.
    /** Element type. */
    typedef Policy value_type;
    /** Type of pointers to elements. */
    typedef Policy* pointer;
    /** Type of references to elements. */
    typedef Policy& reference;
    /** Iterator category. */
    typedef std::input_iterator_tag iterator_category;
    /** Difference between iterators */
    typedef std::ptrdiff_t difference_type;

    policy_iterator() {
    }

	 Policy operator*() const{
		return fetch();
    }

    policy_iterator& operator++(){
		++idx_;
		return *this;
    }

    bool operator==(const policy_iterator& a) const{
		return idx_==a.idx_;
    }
   
	 bool operator<(const policy_iterator& a) const{
		return idx_<a.idx_;
    }
   private:
    friend class Sampler;
    	Sampler* set_;
    	size_type idx_;

		policy_iterator(const Sampler* set, size_type idx)
		  : set_(const_cast<Sampler*>(set)),idx_(idx) {
		}

		Policy fetch() const{
		size_type a = 0;
			for (auto it = set_->used_id_set_.begin(); it != set_->used_id_set_.end(); ++it){
				if (a==idx_)
					return set_->policy(*it);
				a += 1;
			}
			return set_->policy(set_->max_num_policies());
		}
  };

  private:
		/** Stores Sample Information */
		struct sample_info_type {
			bool meets_policy_;
			sample_value_type value_;
		};

		/*Info stored for each policy*/
		struct policy_info_type{
			size_type max_num_samples_;
			size_type max_size_;
			bool status_; 
			clock start_t_;
			clock end_t_;
			policy_value_type value_;
			policy_info_type(): max_num_samples_(1000),max_size_(sizeof(policy_value_type)),status_(0),start_t_(clock()),end_t_(clock()),value_(policy_value_type()){}
		};

		/**Maximum number of outstanding policies that can be defined at one time**/
		size_type max_policies_;

		/**Information about state of all policies**/
		size_type num_active_policies_; 
		size_type num_inactive_policies_;

		/**maps policy_ids to samples**/
		map<size_type,GraphType> policy2samples_;
		map<size_type,policy_info_type> policies_;
	
		/**Policy id (pid) pool**/		
		unordered_set<size_type> free_id_set_; 
		unordered_set<size_type> used_id_set_; 

		/** Returns true if a policy object is a valid object
		*/
		bool is_valid(Policy& p) const{
			size_type pid = p.get_id();
			for(auto it=used_id_set_.begin(); it!=used_id_set_.end(); ++it)
				if((*it)==pid) return true;
			return false;
		}

		/**Returns a Policy object**/
	  	Policy policy(size_type id){
			return Policy(this,id);
		} 

};







