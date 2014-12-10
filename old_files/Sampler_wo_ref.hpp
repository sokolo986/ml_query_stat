/** Sampler Class
 ** Collected & maintains samples IAW a registered policy
 ** Samples are associated with a policy; therefore, policies
 ** must be registered before samples are requested
 **/
/*Define a sample-wide and policy-wide constraint*/

// FIX UCHAR_MAX -- should be the max of size_type and size_type should be templated
// Fix Container.hpp


#include "CS207/Util.hpp"
#include "Point.hpp"
#include "Graph.hpp"
#include <vector>
#include <map>
#include <unordered_set>
#include <limits.h>
using namespace std;

/*Requirements for p: p must define a 'collect' and 'stopcollect' method, C is the return type for the collect routine, S is the class/struct for samples*/
template <typename P, typename S, typename C>
class Sampler{

 private:
	/**Define an edgeless graph to hold samples**/
	struct edge_null {
	unsigned edge;
	};
	typedef edge_null E;
	typedef Graph<S,E> GraphType;

	/**Holds policy and sample information**/
	struct policy_info_type;
	struct sample_info_type;
	
 public:

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

	/*Debugging purposes*/
	void print_stats(){
		cout << "Max policies:" << max_policies_ << '\n';
		cout << "Active policies:" << num_active_policies()<< '\n';
		cout << "Inactive policies Var:" << num_inactive_policies_ << '\n';
		cout << "Max policies: " << policies_.size() << '\n';
		cout << "Max policies using policy2sample:" << policy2samples_.size()<< '\n';
		cout << "Num policies: " << num_policies() << '\n';
		cout << "Num used ids (act/inc policies):" << (num_active_policies_+num_inactive_policies_) << '\n';
	}

	void free_map(){
		cout << "Free:";
		for (const size_type& x: free_id_set_){
			cout << "  " << x;
		}
		cout << endl;
	}

	void used_map(){
		cout << "Used:";
		for (const size_type& x: used_id_set_){
			cout << "  " << x;
		}
		cout << endl;
	}

/**-------------------Policy-wide methods (perform actions for multiple policies)*/

	/** Starts the collection of all samples
	* @pre			is_valid(p)
	* return			(for i in [0,max_policies()) !p.is_active() && 0) && (1 && there exists a p for which is_active() is true)
	* @post			num_active_policies()==0;
	*/
	bool start_collections() {
		for(auto it = policies(); it != policies_end(); ++it)
			start_collection(*it);
		assert (num_inactive_policies()==0);
		return (num_inactive_policies()!=0);
	}

	/** Stops the collection of all samples
	* @pre			is_valid(p)
	* return			(for i in [0,max_policies()) !p.is_active() && 0) && (1 && there exists a p for which is_active() is true)
	* @post			num_active_policies()==0;
	*/
	size_type stop_collections() {
		for(auto it = policies(); it != policies_end(); ++it)
			stop_collection(*it);
		assert (num_active_policies()==0);
		assert (num_inactive_policies()==used_id_set_.size());
		return (num_active_policies()!=0);
	}

		/** Increases max_policy() limit
	* @pre			max_policies()+num_policies_to_add < UCHAR_MAX
   * @post 			max_policies()+=num_policies_to_add
	*/
	void increase_num_policies(size_type num_policies_to_add){
		assert((max_num_policies()+num_policies_to_add)< UCHAR_MAX);
		
		for(size_type i=max_num_policies(); i<(max_num_policies()+num_policies_to_add); ++i)
			free_id_set_.insert(i);

		for (size_type i=0; i<max_num_policies; ++i)
			policies_[i] = policy_info_type();

		for (size_type i=0; i<max_num_policies; ++i)
			policy2samples_[i] = GraphType();

		num_inactive_policies_ +=num_policies_to_add;
		max_policies_ += num_policies_to_add; 
	}

	/** Creates a new policy
	* @pre			num_policies()!=max_policies()
	* return			vaid Policy object
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/
  	Policy create_policy(policy_value_type pv=policy_value_type()){
		assert(num_policies()!=max_num_policies());		
		auto it = free_id_set_.begin();
		size_type pid = *it;
		free_id_set_.erase(it);
		used_id_set_.insert(pid);

		++num_inactive_policies_;
		policies_[pid].value_ = pv;
		policy2samples_[pid] = GraphType();
		assert(policy2samples_.size()==policies_.size());
		return Policy(this,pid);
 	} 

	/** creates multiple policies at once
	* @pre			num_policies+policies() < max_policies()
   * @post 			num_policies += max_policies()
	*/
	void create_policies(size_type num_policies){
		for(size_type i=0; i < num_policies; ++i)
			create_policy();
	}

	/**Returns a Policy object**/
  	Policy policy(size_type id){
		return Policy(this,id);
	} 

	/**Returns an iterator to the start of all policies**/
	policy_iterator policies(){
		return policy_iterator(this, 0);
	}

	/**Returns an iterator to the end of all policies**/
	policy_iterator policies_end(){
		return policy_iterator(this, num_policies());
	}

/**-------------------Policy-wide stats (Stats for the entire sampler and ones that affect multiple policies)*/

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

	/**Returns the remaining number of unassigned policy ids**/
	size_type total_unused_ids(){
		return max_policies_ - num_policies();
	}


/**-------------------Policy-level methods (affect an individual policy)*/

	/** Restarts the sample collection process for an existing VALID policy
	* @pre			is_valid(p)==True
 	* @param[in]	max_policies  i in [1,UCHAR_MAX)
	* return			0 or 1
   * @post 			(num_active_policies() += 1 && num_policies() += 1)  || (no change to num_active_policies() and num_policies())
	*/
	bool start_collection(Policy p) {
		assert(is_valid(p));
		if (p.is_active()) return 1;
		if (p.has_policy_met_limit()) return 1;
		p.status() = 1;
		p.start_t() = clock(); 
		--num_inactive_policies_;
		++num_active_policies_;
		assert(num_active_policies_==num_active_policies());
		p.collect();
		assert ((num_active_policies_+num_inactive_policies_)==used_id_set_.size());
		return 0;
	}

	/** Start the sample collection process for a policy. Calls the collect() function for a policy
	* @pre			pv.collect() and pv.stop_collect() exist
	* return			0 or 1
   * @post 			num_active_policies() += 1 && num_policies() += 1)  
	*/  
	collect_return_type start_collection(policy_value_type pv) {
		Policy p = create_policy(pv);		
		assert(is_valid(p));
		if (p.is_active()) return;
		if (p.has_policy_met_limit()) return;
		p.status() = 1;
		p.start_t() = clock(); 
		--num_inactive_policies_;
		++num_active_policies_;
		assert(num_active_policies_==num_active_policies());
		return p.collect();
		//return 0;

		//return start_collection(create_policy(pv));
	}

	/** Stops sample collection for a specific policy
	* @pre			is_valid(p)
	* return			(p.is_active() && 0) || (!p.is_active() && 1)
   * @post 			if p.is_active() -> num_active_policies() -= 1
	* @post			if p.is_active() ->num_inactive_policies()+= 1
	* @post			p.end_t() > p.start_t()
	* @post			!p.is_active();
	*/
	size_type stop_collection(Policy p){
		if (!p.is_active()) return 1;
		p.stop_collect();
		p.end_t() = clock();
		p.status() = 0;
		++num_inactive_policies_;
		--num_active_policies_;
		return 0;
	}

	/** Returns true if a policy object is a valid object
    */
	bool is_valid(Policy& p) const{
		size_type pid = p.get_id();
		for(auto it=used_id_set_.begin(); it!=used_id_set_.end(); ++it)
			if((*it)==pid) return true;
		return false;
	}

	/** Marks all samples if they meet a policy
	* @pre			num_policies()!=max_policies()
	* return			vaid Policy object
   * @post 			num_policies() += 1
	* @post			num_inactive_policies() += 1
	*/
	void mark_if_meets_policy(Policy& p){
		p.mark_if_meets_policy();
	}

	/** Enforces a policy-wide constraint and sample-wide constraint -- ripe for parallelizing
	* @pre			all samples are marked if they meet a policy!! Do not fail to do this step or all samples may be erased
	* return			0 if all samples in p meet policy; 1 if 1 or more does not
   * @post 			all samples adhere to policy and sample-wide constraints
	*/
	void enforce_policy(Policy& p){
		p.enforce_policy();
	}

  	/** Copy and create policy using the value from another policy
	*/
  Policy copy_policy(Policy& p) {
	return create_policy(p.value());
  } 

	/** Create a new policy using the samples from another policy that meet the policy constraints
	*/
  	Policy copy_samples_that_meet_policy(Policy& p) {
		Policy q = create_policy(p.value());
		mark_if_meets_policy(p);
		for (auto it = samples(); it != samples_end(); ++it)
			if ((*it).meets_policy_)
				q.add_sample((*it).value_);
		return q;
  	} 

	/** Deletes a specific sample
	* @pre			is_valid(p)
	* @post 			num_samples(p)==0
	* @post			sample_id may no longer be valid after deletion
	*/
	bool delete_sample(Policy& p, size_type sample_id){
		assert(is_valid(p));
		size_type pid  = p.get_id();
		assert(policy2samples_[pid].size() > sample_id);
		size_type init = policy2samples_[pid].size();
		policy2samples_[pid].remove_node(sample_id);
		return init==policy2samples_[pid].size();
	}

	/** Deletes all samples from a policy
	* @pre			is_valid(p)
	* @post 			num_samples(p)==0
	*/
	void delete_samples(Policy& p){
		assert(is_valid(p));
		policy2samples_[p.get_id()].clear();
	}

	/** Deletes a policy. Invalidates all existing pointers
	* @pre			is_valid(p)
	* @post 			invalidated all existing pointers/references to outstanding policies
	*/
	void delete_policy(Policy& p){
		if (p.is_active())
			--num_active_policies_;
		else
			--num_inactive_policies_;
		policy2samples_[p.get_id()].clear();
		policies_[p.get_id()] = policy_value_type();
		free_id_set_.insert(p.get_id);
		used_id_set_.erase(p.get_id);
	}

	void delete_policies(){
		clear();
	}

	/**Returns the policy referenced by an iterator**/
	Policy& policy(policy_iterator it){
		return *it;
	}

  	/**Returns begin iterator for samples in a policy**/
  	sample_iterator samples(Policy& p) {
		return policy2samples_[p.get_id()].node_begin();
  	}

	/**Returns end iterator for samples in a policy**/
	sample_iterator samples_end(Policy& p){
		return policy2samples_[p.get_id()].node_end();
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


/*------------Getters and Setters for policies */

	/** Gets the ID of a policy
	* @pre			is_valid(p)
   * return			[0,max_policies())
	*/
	size_type get_id(Policy& p) const{
		assert(is_valid(p));
		size_type pid = p.get_id();
		assert(pid < max_num_policies());
		return p.get_id();
	}

	/** Edits an existing policy but does not delete samples that do no adhere
	* @pre			is_valid(p)
   * @post 			p.value() = pv
	*/
	policy_value_type& policy_value(Policy& p) {
		return p.value();
	}

	/**Returns number of samples in a policy**/
	size_type num_samples(Policy& p) {
		return policy2samples_[p.get_id()].size();
	}

	/**Returns number of samples in a policy**/
	size_type max_num_samples(Policy& p) {
		return p.max_samples();
	}


  /** Sampling Policy determines how to collect samples and which samples to collect**/
  class Policy:private totally_ordered<Policy> {
   Policy(){
   }

	/**Mimics a sampling process --- must call the collect of a policy_value_type**/
   collect_return_type collect() const{
		return fetch().value_.collect();
   }

	/**Mimics a sampling process --- must call the stop_collect of a policy_value_type (user must implement)**/
   collect_return_type stop_collect() const{
		return fetch().value_.stop_collect();
   }

	clock& start_t() const{
		return fetch().start_t_;
	}

	clock& end_t() const{
		return fetch().end_t_;
	}

   bool is_active() const{
		return fetch().status_;
   }

   size_type get_id() const{
		return uid_;
   }

	size_type& max_size() const{
		return fetch().max_size_;
	}

	size_type& max_samples() const{
		return fetch().max_num_samples_;
   }

   policy_value_type& value() const{
		return fetch().value_;
   }

   void add_sample(sample_value_type sv){
		Sample s = fetch_samples().add_node(Point());
		s.value() = sv;
   }

	//policy-wide constraints
	bool has_policy_met_limit(){
		return ((max_samples() <= num_samples()) && (max_size() <= sizeof(sample_value_type)*num_samples())); 
	}

	//mark samples that meet constraints
	bool mark_if_meet_constraints(){
		return fetch().value_.mark_if_meet_constraints();
	}

	bool enforce_policy(){
		return fetch().enforce();
	}


   void delete_sample(size_type sid){
		Sample s = GraphType::node(sid);
		fetch_samples().delete_node(s);
   }

   sample_iterator sample_start(){
		return fetch_samples().node_begin();
   }

	sample_iterator samples_end(){
		return fetch_samples().node_end();
	}

	sample_value_type& sample(sample_iterator s){
		return (*s).value();
	}

   sample_value_type& sample(size_type sid){
		return fetch_samples().node(sid).value();
   }

	sample_value_type& sample(Sample& s){
		return fetch_samples().s.value();
   }

   size_type num_samples() {
		return set_->policy2samples_[uid_].size();
   }

	void meets_policy(bool meets){
		fetch().meets_policy_ = meets;
	}

   void clear(){
		stop_collect();
		fetch_samples().clear();
   }

	bool operator==(Policy& a){
		return a.uid_==(uid_);
	}

	bool operator<(Policy& a){
		return a.uid_<(uid_);
	}

   private:
		friend class Sampler;
		Sampler* set_;
		size_type uid_;

		Policy(const Sampler* set, size_type uid)
		  :set_(const_cast<Sampler*>(set)),uid_(uid){
		}

		policy_info_type& fetch(){
			return set_->policies_[uid_];
		}

		policy_info_type& fetch() const{
			return set_->policies_[uid_];
		}

		policy_info_type& fetch_samples(){
			return set_->policy2samples_[uid_];
		}

		size_type& get_id(){
			return uid_;
		}

		bool& status(){
			return fetch().status_;
		}

		size_type& max_size(){
			return fetch().max_size_;
		}

		size_type& max_samples(){
			return fetch().max_num_samples_;
		}

		size_type& samples(){
			return fetch_samples().size();
		}

		clock& start_t(){
			return fetch().start_t_;
		}

		clock& end_t(){
			return fetch().start_t_;
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
			Policy* set_; 
			bool meets_policy_;
			sample_value_type value_;
		};

		/** Stores Policy Information 
		  * status defines active or inactive status
		  */
		struct policy_info_type{
			size_type max_num_samples_;
			size_type max_size_;
			bool status_; 
			clock start_t_;
			clock end_t_;
			policy_value_type value_;
			policy_info_type(): max_num_samples_(1),max_size_(sizeof(policy_value_type)),status_(0),start_t_(clock()),end_t_(clock()),value_(policy_value_type()){}
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
};







