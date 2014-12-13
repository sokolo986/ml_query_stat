/** meets domain: returns true if a sample meets a domain
struct model parameters error from training 
 ** currently has support for up to 4 different models
 */
template <typename M1, typename M2 = M1, typename M3 = M1, typename M4 = M1>
class Manager{
 private:
	//struct edge_null {
	//	unsigned edge;
	//};
	//typedef edge_null E;

	/**Holds policy and sample information**/
	//struct model_info_type;
	//typedef model_info_type O;//same as node value type
	struct model_info_type;

 public:
	

	/** Graph typedefs **/
	//typedef Graph<M,E> GraphType;
	//typedef typename GraphType::size_type size_type;

	typedef unsigned size_type;
	//typedef typename GraphType::Node Model;
	//typedef typename GraphType::Node model_type;
	//ypedef typename GraphType::node_value_type model_info_type;
	//typedef typename GraphType::node_iterator model_iterator;
	typedef Manager manager_type;

	Manager():models_(),used_m1_(0),used_m2_(0),used_m3_(0),used_m4_(0){
	}

   ~Manager() = default;

   Manager& operator=(const Manager&) = delete;

	size_type num_models(){
		return ((size_type) (used_m1_ + used_m2_ + used_m3_ + used_m4_)); 
	}

	void add_model(M1 m1 = M1(), M2 m2 = M2(), M3 m3 = M3(), M4 m4=M4()){
		if (m1 != M1()){
			models_.m1_ = m1;
			used_m1_ = 1;
		}
		if (m2 != M2()){
			models_.m2_ = m2;
			used_m2_ = 1;
		}
		if (m3 != M3()){
			models_.m3_ = m3;
			used_m3_ = 1;
		}
		if (m4 != M4()){
			models_.m4_ = m4;
			used_m4_ = 1;
		}
	}

	void clear_model(size_type id){
		assert(id < 5 && id > 0);
		if (id == 1){
			models_.m1_ = M1();
			used_m1_ = 0;
		}
		if (id == 2){
			models_.m2_ = M2();
			used_m2_ = 0;
		}
		if (id == 3){
			models_.m3_ = M3();
			used_m3_ = 0;
		}
		if (id == 4){
			models_.m4_ = M4();
			used_m4_ = 0;
		}
	}

	M1 model1(){
		return models_.m1_;
	}

	M2 model2(){
		return models_.m2_;
	}

	M3 model3(){
		return models_.m3_;
	}

	M4 model4(){
		return models_.m4_;
	}

	

/*	M& model_value(Model m){
		return m.value().mv_;
	}*/

	void clear(){
		models_.m1_ = M1();
		models_.m2_ = M2();
		models_.m3_ = M3();
		models_.m4_ = M4();
		used_m1_ = 0;
		used_m2_ = 0;
		used_m3_ = 0;
		used_m4_ = 0;
	}


	private:

		/*struct model_info_type{
			error_type emp_risk_;//risk
			M mv_; 					//parameters
								//user defined domain of model
		};*/
		/**Policy_info_type must have the same**/
		//vector<M> models_;
		//vector<size_type> model2uid_; //stores official uids associated with policies_
		struct model_info_type{
			M1 m1_;
			M2 m2_;
			M3 m3_;
			M4 m4_;
			model_info_type():m1_(M1()),m2_(M2()),m3_(M3()),m4_(M4()){
			}
		};
		
		model_info_type models_;

		//GraphType* graph_;
		bool used_m1_;
		bool used_m2_;
		bool used_m3_;
		bool used_m4_;

};
