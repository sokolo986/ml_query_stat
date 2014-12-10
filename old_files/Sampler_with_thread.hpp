/** Sampler Class
 ** Collected & maintains samples IAW a registered policy
 ** Samples are associated with a policy; therefore, policies
 ** must be registered before samples are requested
 **/
#include "CS207/Util.hpp"
#include "Point.hpp"
#include "Graph.hpp"
//#include <algorithm>
#include <vector>
#include <cassert>
#include <map>
#include <thread>
#include <unordered_set>
#include <chrono>
#include <limits.h>
using namespace std;

template <typename P, typename S>
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
	** @max_policies	[1,UCHAR_MAX)
	*/
	Sampler(size_type max_policies){
		//number of policies cannot exceed UCHAR_MAX
		assert((max_policies >= 1) && (max_policies<UCHAR_MAX));

		//create pool of policy ids as appropriate  
		for (size_type i=0; i<max_policies; ++i)
			free_id_set_.insert(i);

		for (size_type i=0; i<max_policies; ++i)
			policies_.push_back(i,policy_info_type());

		num_inactive_policies_ = free_id_set_.size();
		num_active_policies_ = 0;
		max_policies_ = max_policies;
	}

	/** Use Default destructor **/
	~Sampler() = default;

  /**Restarts the sample collection process for an existing VALID policy
   **output: [0 && success, 1 && failure]
   **returns 0 || 1 
   */
	size_type start_collection(Policy& p) {
		//If policy is running, nothing to do
		if (p.is_active()) return 1;
		if (has_policy_met_constraints(p)) return 1;

		p.start_time() = clock::now(); //fix this
		std::thread thr(p.collect());
		p.thr()=thr;
		p.status()=1;

		if (p.thr()){
			--num_inactive_policies_;
			++num_active_policies_;
			return 0;
		}else{
			return 1;
		}
	}

	/**If given a policy_value_type instead, starts a new collection**/      
	size_type start_collection(policy_value_type pv) {
		Policy p = create_policy(pv);
		return start_collection(p); 	
	}

	/*returns 1 if policy constraints have been met and 0 otherwise*/
	bool has_policy_met_constraints(Policy& p){
		//policy is already satisfied - number of samples has been met
		if (p.max_num_samples() <= p.num_samples()) return 1; 

		//policy memory constraints have been exhausted
		if (p.max_size() <= sizeof(sample_value_type)*p.num_samples()) return 1; 

		return 0;
	}

	/**Stops sample collection for a specific policy
	**returns policy_id 0 if success or 1 if failure **/
	size_type stop_collection(Policy& p){
		if (!p.is_active()) return 1;
		//no thread to stop
		if (p.thr()) return 1;

		~p.thr();
		p.end_time() = clock::now(); //fix this

		++num_inactive_policies_;
		--num_active_policies_;
		return 0;
	}

   /**Stops sample collection for all policies
   **returns 0 if success or 1 if failure **/
	size_type stop_collections(bool verify_check=0) {
		for(auto it = policies().begin(); it < policies().end(); it++)
			stop_collection(*it);

		if (num_active_policies_== 0){
			if (verify_check){
				for(auto it = policies().begin(); it < policies().end(); it++)
					if ((*it).status()) return (*it).status();
			}
			return 0;
		}
		return 1;
	}

  /**Creates a new policy**/
  Policy& create_policy(policy_value_type pv=policy_value_type()) {
		size_type pid = free_id_set_.pop();
		used_id_set_.push_back(pid);

		++num_inactive_policies_;

		policy2samples_[pid] = GraphType();
		policies_[pid] = pv;
		return Policy(this,pid);
  } 

  /**Copies existing policy and returns the id of a new copy
   **Note: This only copies the policy and not the samples**/
  Policy& copy_policy(Policy p) {
	return create_policy(p.value());
  } 

  /**Edits an existing policy but does not delete samples that 
   **do not adhere to the new policy**/
  void set_policy(Policy& p, policy_value_type pv){
	  p.value()=pv;
  }

  /**Enforces a policy and deletes all samples that do not adhere to the policy**/
	void enforce_policy(Policy& p){
		for (auto it=samples(p).begin(); it<samples(p).end(); ++it){
			if (!p.pred(*it))
				p.delete_sample(*it);
		}
		//test max size and max MB
	}

	void increase_num_policies(size_type num_policies_to_add){
		//creating another policy would violate memory constraints
		if (this->max_policies()== this->num_policies()) return 1; 	
		for(size_type i=(max_policies_+1); i<(max_policies_+1+num_policies_to_add); ++i)
			free_id_set_.push_back(i);
		num_inactive_policies_ += num_policies_to_add;
		max_policies_ += num_policies_to_add; 
	}

  /**Deletes a policy and all of its samples**/
  void delete_policy(Policy& p){
	size_type pid = p.get_id();
	if (p.is_active())
		--num_active_policies_;
	else
		--num_inactive_policies_;
	used_id_set_.erase(pid);
	policy2samples_.erase(pid);
	policies_.erase(pid);
  }

  /**Deletes all samples from a policy**/
  void delete_samples(Policy& p){
	size_type pid  = p.get_id();
	policy2samples_[pid].clear();
  }

  /**Deletes a specific sample from a policy
	**returns [1 && success || 0 && failure]
	*/
  bool delete_sample(Policy& p, size_type sample_id){
	size_type pid  = p.get_id();
	size_type init = policy2samples_[pid].size();
	policy2samples_[pid].remove_node(sample_id);
	return init==policy2samples_[pid].size();
  }

	/**Returns max number of policies allowed */
	size_type max_num_policies(){
		return max_policies_; 
	}


	/**Returns information about a specific policy**/
	policy_value_type& policy_value(Policy& p) {
		return p.value();
	}

  /**Returns a Policy object**/
  Policy& policy(size_type id){
	return Policy(this,id);
  } 

  size_type pid(Policy& p){
	return p.get_id();
  }

  /**Returns an iterator to all policies**/
  policy_iterator policies(){
	return policies_.begin();
  }

  policy_iterator policies_end(){
	return policies_.end();
  }

  Policy& policy(policy_iterator it){
	return *it;
  }

  /**Return sample iterator for samples in a policy**/
  sample_iterator samples(Policy& p) {
	return policy2samples_[p.get_id()].node_begin();
  }

  /**Erases all policies and samples**/
  void clear(){
   	for(auto it=policies(); it < policies_end(); ++it)
			delete_samples(*it);
		num_inactive_policies_ = 0;
		num_active_policies_ = 0;
		policy2samples_.clear();
		policies_.clear();
  }

  /**Returns the number of policy that are actively collecting samples**/
  size_type num_active_policies(){
	return num_active_policies_;
  }

	/**Returns the number of policy that are not actively collecting samples**/
	size_type num_inactive_policies(){
		return num_inactive_policies_;
	}

	/**Total active policies**/
	size_type num_policies() {
		assert(policies_.size()==(num_active_policies_+num_inactive_policies_));
		return policies_.size();
	}

	/**Returns number of samples in a policy**/
	size_type num_samples(size_type policy_id) {
		return policy2samples_[policy_id].size();
	}

	/**Returns the remaining number of unassigned policy ids**/
	size_type total_unused_ids(){
		return max_policies_ - num_policies();
	}

  /** Sampling Policy determines how to collect samples and which samples to collect**/
  class Policy {
   Policy(){
   }

   void collect() const{
		set_->start_collection(*this);
   }

   void stop_collect() const{
		set_->stop_collection(*this);
   }

	clock& start_t() const{
		return fetch().start_t_;
	}

	clock& end_t() const{
		return fetch().end_t_;
	}

	size_type& max_size() const{
		return fetch().max_size_;
	}

	size_type& max_samples() const{
		return fetch().max_num_samples_;
   }

   bool is_active() const{
		return fetch().status_;
   }

   size_type get_id() const{
		return uid_;
   }

   policy_value_type& value() const{
		return fetch().value_;
   }

	void set_value(const sample_value_type& s) const{
		fetch().value_ = s;
   }

   void add_sample(sample_value_type sv){
		Sample s = fetch_samples().add_node(Point());
		s.value() = sv;
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

   sample_value_type& sample(size_type sid){
		return fetch_samples().node(sid).value();
   }

	sample_value_type& sample(Sample& s){
		return fetch_samples().s.value();
   }

   size_type num_samples(){
		return policy2samples_[uid_].size();
   }

   void clear(){
		stop_collect();
		fetch_samples().clear();
   }

   private:
		friend class Sampler;
		Sampler* set_;
		size_type uid_;

		Policy(const Sampler* set, size_type uid)
		  :set_(const_cast<Sampler*>(set)),uid_(uid) {
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

		std::thread& thr(){
			return fetch().thr_;
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
		return set_->policy(idx_);
    }

    policy_iterator& operator++(){
		++idx_;
		return *this;
    }

    bool operator==(const policy_iterator& a) const{
		return (idx_==a.idx_ && set_==a.set_);
    }
   
   private:
    friend class Sampler;
    Sampler* set_;
    size_type idx_;

    policy_iterator(const Sampler* set, size_type idx)
        : set_(const_cast<Sampler*>(set)),idx_(idx) {
    }
  };

  private:
		/** Stores Sample Information */
		struct sample_info_type {
			Policy* set_; 
			sample_value_type value_;
		};

		/** Stores Policy Information 
		  * If the thread=0 then we know it is not assigned 
		  * status defines active or inactive status
		  */
		struct policy_info_type{
			Sampler* set_; 
			size_type max_num_samples_;
			size_type max_size_;
			bool status_; 
			clock start_t_;
			clock end_t_;
			thread t_;
			policy_value_type value_;
			policy_info_type(): set_(this),max_num_samples_(1),max_size_(sizeof(policy_value_type)),status_(0),start_t_(clock()),end_t_(clock()),t_(thread()),value_(policy_value_type()){}
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







