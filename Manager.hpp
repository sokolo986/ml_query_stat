/** meets domain: returns true if a sample meets a domain
struct model parameters error from training */


//M = model_value_type and D = domain_value_type
template <typename M, typename D>
class Manager{
 private:
	struct edge_null {
		unsigned edge;
	};
	typedef edge_null E;

	/**Holds policy and sample information**/
	struct model_info_type;
	typedef model_info_type O;//same as node value type
	

 public:

	typedef D domain_value_type;
	typedef M model_value_type;
	class model_iterator;
	/** Graph typedefs **/
	typedef Graph<O,E> GraphType;
	typedef typename GraphType::size_type size_type;
	typedef double error_type;
	typedef typename GraphType::Node Model;
	typedef typename GraphType::Node model_type;
	typedef typename GraphType::node_value_type model_info_type;
	//typedef typename GraphType::node_iterator model_iterator;
	typedef std::chrono::high_resolution_clock clock;
	typedef Manager manager_type;

	Manager(): graph_(new GraphType()){
	}

   ~Manager() = default;

   Manager& operator=(const Manager&) = delete;

	size_type num_models(){
		return graph_->size();
	}

	Model add_model(M mv){
		model_info_type mi{DBL_MAX,mv,D()};
		return graph_->add_node(Point(),mi);
	}

	void remove_model(Model m){
		graph_->remove_node(m);
	}

	void remove_model(size_type id){
		graph_->remove_node(model(id));
	}

	void remove_model(model_iterator it){
		graph_->remove_node(*it);
	}

	model_iterator model_begin(){
		return model_iterator(graph_,0);
	}

	model_iterator model_end(){
		return model_iterator(graph_,graph_->size());
	}

	Model model(model_iterator it){
		return (*it);
	}

	Model model(size_type id){
		return graph_->node(id);
	}

	size_type model_id(Model m){
		return m.index();
	}

	M& model_value(Model m){
		return m.value().mv_;
	}

	error_type& model_risk(Model m){
		return m.value().emp_risk_;
	}

	D& model_domain(Model m){
		return m.value().dv_;
	}

	void reset_model (Model& m){
		model_error(m)  = error_type();
		model_param(m)  = M();
		model_domain(m) = D();
	}

	void clear(){
		graph_->clear();
	}


	class model_iterator :private totally_ordered<model_iterator>{
    public:
    typedef Model value_type;
    typedef Model* pointer;
    typedef Model& reference;
    typedef std::input_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

    model_iterator() {
    }

    Manager::Model operator*() const{
        return graph_->node(it_);
    }

    model_iterator& operator++(){
        ++it_;
        return *this;
    }

    bool operator==(const model_iterator& mit) const {
        return mit.it_==it_;
    }

    bool operator<(const model_iterator& mit) const {
        return it_ < mit.it_;
    }

   private:
    friend class Manager;

    Manager::GraphType* graph_;
    size_type it_;

    model_iterator(const Manager::GraphType* graph,size_type id):graph_(const_cast<Manager::GraphType*>(graph)), it_(id){};
  };

	private:

		struct model_info_type{
			error_type emp_risk_;//risk
			M mv_; 					//parameters
			D dv_; 					//user defined domain of model
		};

		GraphType* graph_;

};
