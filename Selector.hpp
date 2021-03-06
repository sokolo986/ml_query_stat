
/** Meets domain: returns true if a sample meets a domain
struct model parameters error from training **/

/**M = model_value_type and D = domain_value_type
	X = value type for the features
	Y = value_type for the predicted value
	L = loss function to determine the risk of a model**/
template <typename M, typename X, typename Y>
class Selector{
//add Cross_validation to this
 public:

	/** Model value type **/
	typedef M model_value_type;
	//typedef D domain_value_type;
	typedef X X_value_type;
	typedef Y y_value_type;

	/** Graph typedefs **/
	typedef Manager<M> ManagerType;
	typedef typename ManagerType::size_type size_type;
	typedef typename ManagerType::error_type error_type;
	typedef typename ManagerType::Model Model;
	typedef typename ManagerType::Model model_type;
	typedef typename ManagerType::model_iterator model_iterator;
	typedef std::chrono::high_resolution_clock clock;
	typedef Selector selector_type;

	//user should define cv constant
	Selector(){
	}

   ~Selector() = default;

   Selector& operator=(const Selector&) = delete;

	void train(ManagerType mt, Model m, X& x, Y& y){
		mt.model_value(m).train(x,y);
	}

	Y predict(ManagerType mt, Model m, X& x){
		return mt.model_value(m).predict(x);
	}

	//perform cross validation

	error_type calculate_loss(ManagerType mt, Model m, Y& y1, Y& y2){
		return mt.model_value(m).loss(y1,y2);
	}

	Model best_model(ManagerType mt, X& x_train, X& x_test, Y& y_train, Y& y_test){
		double min_loss = DBL_MAX;
		model_iterator best_model = ManagerType::Model();
		Y y_hat;

		for(auto it = mt.model_begin(); it < mt.model_end(); ++it){
			train(mt,(*it),x_train,y_train);			
			y_hat = predict(mt,(*it),x_test);
			error_type loss = calculate_loss(mt,(*it),y_test,y_hat);
			mt.model_risk(*it) = loss;
			if (loss < min_loss){
				min_loss = loss;
				best_model = it;
			}
		} 
		return *best_model;
	}

};
