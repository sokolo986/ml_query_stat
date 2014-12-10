/** Sampler Class
 ** Collected & maintains samples IAW a registered policy
 ** Policies define HOW and WHAT samples will be collected.

 ** typename S must define a 'collect(clock start_time, clock end_time)' routine, where the start and end time define the time period to collect samples within

 ** if sample_value_type collect() returns too many samples, the first @ a samples within the maximum number of samples limit will be stored

 //type S can be of type float/int/double
 ** to free up policies from time to time clear container 
 **/


template <typename P, typename S>
class Sampler{
 private:

	/**Holds policy and sample information**/
	struct policy_info_type;

 public:
	/** Value types **/
	typedef P policy_value_type;
	
	/** Graph typedefs **/
	class Policy;
	class policy_iterator;
	typedef Sampler sampler_type;
	
	typedef std::vector<S> Samples;
	typedef Policy policy_type;
	typedef unsigned size_type;
	typedef std::chrono::high_resolution_clock clock;

	/** Constructor for Sampler Class
	*/
	Sampler(){
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

	/** Creates a new policy
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/

	/**collect sec delta is the maximum number of seconds that each collection should look for samples in*/
  	Policy create_policy(size_type max_samples = 1000, size_type collect_sec_delta = 60){
		policy_info_type p (max_samples,false,clock(),clock(),collect_sec_delta,policy_value_type());//,Samples());

		policies_.push_back(p);
		policy2uid_.push_back(policies_.size()-1);
		return Policy(this,policy2uid_.size()-1);
 	} 

	/** Creates a new policy copying the samples from an existing policy
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/
  	Policy create_policy(Policy& p){ //added to policy
		assert(is_valid(p));
		Policy p1 = p;
		policies_.push_back(p1);
		policy2uid_.push_back(policies_.size()-1);
		return Policy(this,policy2uid_.size()-1);
 	} 

	/** Deletes a policy. Invalidates all existing pointers
	* @post 			invalidated all existing pointers/references to outstanding policies
	*/
	void delete_policy(Policy& p){
		assert(is_valid(p));
		size_type pid = p.get_id();

		size_type lookupId = policy2uid_[pid];
		policies_[lookupId] = policy_info_type();
		policy2uid_.erase(policy2uid_.begin()+pid);
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

	Policy policy(size_type id){
		assert(id < num_policies());
		return Policy(this,id);
	}

	/**Total active/inactive policies**/
	size_type num_policies() {
		return policy2uid_.size();
	}

	/**Returns the number of policy that are actively collecting samples**/
	size_type num_active_policies(){
		size_type i = 0;
		for(auto it = policies(); it != policies_end(); ++it)
			if ((*it).is_active())
				i += 1;
		return i;
  	}

	/**Returns the number of policy that are not actively collecting samples**/
	size_type num_inactive_policies() {
		return num_policies()-num_active_policies();
	}

  /**Erases all policies and samples**/
  void clear(){
		policies_.clear();
		policy2uid_.clear();
  }

	void stats(){
		cout << "Sampler Stats" << endl;
		cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		cout << "Total Number of Policies: " << num_policies() << endl;
		cout << "Number Active Policies: " << num_active_policies() << endl;
		cout << "Number Inactive Policies: " << num_inactive_policies() << endl << endl;


		cout << "Individual policy Stats" << endl;
		cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		for(auto it = policies(); it != policies_end(); ++it){
			cout << "Policy id: " << (*it).get_id() << endl;
			cout << "# of samples: " << (*it).num_samples() << endl;
			cout << "Is active?: " << (*it).is_active() << endl;
			cout << "Has met limit?: " << (*it).has_met_limit() << endl;
			cout << "Elements: " << endl;
			(*it).stats();
			cout << endl;
		}

	}

  class Policy:private totally_ordered<Policy> {
	public:
   Policy(){
   }

	/*-----------Samples in -------------*/

	void start_collection() {
		if (is_active()) return;
		fetch().status_ = 1;
		start_t() = clock(); 
		fetch().value_.collect();
		end_t() = clock();
		//fetch_samples().insert(fetch_samples().end(),s.begin(),s.end());
	}

	void add_samples(Samples s){
		fetch_samples().insert(fetch_samples().end(),s.begin(),s.end());
	}

	void add_samples(string filename){
		//TODO
	}

	void collect() {
		start_collection();
	}

	/*-----------Samples out -------------*/

	Samples get_samples(){
		return fetch_samples();
	}

	void get_samples(Sampler::Samples c){
		c.insert(c.end(),fetch_samples().begin(),fetch_samples().end());
	}

	void get_samples(string filename){
		//TODO
	}

	/*-----------Helpers-------------*/

	size_type get_id(){
		return uid_;
	}

	policy_value_type& value(){
		return fetch().value_;
   }

	bool& status() const{
		return fetch().status_;
   }

	bool& is_active() const{
		return status();
	}

	clock& start_t() const{
		return fetch().start_t_;
	}

	clock& end_t() const{
		return fetch().end_t_;
	}

	void stats(){
		fetch().value_.stats();
	}

	bool has_met_limit() {
		return (fetch().value_.has_met_limit() || max_samples() <= num_samples()); 
	}

   void clear(){
		fetch_samples().clear();
   }

	void delete_samples(){
		clear();
	}

	size_type& max_samples(){
		return fetch().max_num_samples_;
   }

	size_type num_samples(){
		return fetch_samples().size();
	}

	bool operator==(Policy& a){
		return a.uid_==(uid_);
	}

	bool operator<(Policy& a){
		return a.uid_<(uid_);
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

		Sampler::Samples& fetch_samples(){
			return set_->policies_[uid_].value_.samples();//samples_;
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

		bool& is_active() {
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
			return set_->policy(idx_);
		}
  };

  private:

		/*Info stored for each policy*/
		struct policy_info_type{
			size_type max_num_samples_;
			bool status_; //is collecting?
			clock start_t_;
			clock end_t_;
			size_type collect_sec_delta_; //time increments for requesting samples (in seconds)
			policy_value_type value_;
			//Samples samples_;
			policy_info_type(): max_num_samples_(1000),status_(0),start_t_(clock()),end_t_(clock()),collect_sec_delta_(60),value_(policy_value_type()){ }//,samples_(Samples()){}

			policy_info_type (size_type max_num_samples, bool status, clock start_t, clock end_t, size_type collect_sec_delta, policy_value_type value){//, Samples samples){
				max_num_samples_ = max_num_samples;
				status_ = status;
				start_t_ = start_t;
				end_t_ = end_t;
				collect_sec_delta_ = collect_sec_delta;
				value_ = value;
				//samples_ = samples;
			}

			void operator=(Policy& p){
				max_num_samples_ = p.max_num_samples_;
				status_ = false;
				start_t_ = clock();
				end_t_ = clock();
				collect_sec_delta_ = p.collect_sec_delta_;
				value_ = p.value_;
				//samples_ = p.samples_;
			}		
		};

		/**Policy_info_type must have the same**/
		vector<policy_info_type> policies_;
		vector<size_type> policy2uid_; //stores official uids associated with policies_

		bool is_valid(Policy& p) const{
			return p.get_id() < num_policies();
		}
};







