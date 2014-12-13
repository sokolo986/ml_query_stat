#include "CS207/Util.hpp"
//#include "Point.hpp"
//#include <algorithm>
#include <vector>
#include <cassert>
//#include <map>
using namespace std;
#include "Sampler.hpp"

//#include <unordered_set>

//#include "Graph.hpp"
#include <float.h>
//#include <limits.h>
#include "Manager.hpp"
#include "Selector.hpp"
//#include <cstdlib>
//#include <math.h>


struct policy_value_type;
struct X_type;
typedef X_type X;
struct Y_type;
typedef Y_type Y;

//template <typename X>
//struct domain_value_type;

typedef vector<float> S;
typedef policy_value_type P;
typedef Sampler<P,S> SamplerType;
typedef SamplerType::Policy policy;
typedef SamplerType::size_type size_type;
//typedef typename SamplerType::Samples C;
typedef S C;

//typedef domain_value_type D;
//Example of using the model manager with two different types of models


struct LearningModelType; //This will be class of models a manager stores
struct Regression_type; //This will be a type of Learning Model;
//typedef LearningModelType<regression_type,X_type,Y_type> Regression;
//typedef Manager<regression_type> ManagerType;

//typedef Selector<Regression,X,Y> SelectorType;
//typedef SelectorType::error_type error_type;
//typedef SelectorType::ManagerType Regression;

//element must have a size() element as well 
//must have an insert method as well
template <typename V,typename Vec>
struct V_type{
	private:
		Vec x_;

	public:

		typedef typename Vec::iterator iterator;

		size_type size() const{
			return x_.size();
		}
		
		void add_vector(Vec t){
			for (size_type i=0; i < t.size(); ++i)
				x_.push_back(t[i]);
		}	

		/*void add_element(Vec::value_type s){
			x_.push_back(s);
		}*/

		bool operator==(const V& y) const{
			return y.x_ == x_;
   	}

		/*void remove_element(size_type idx){
			vector<double> tmp;
			tmp.copy(x_.begin(),x_.begin()+idx);
			tmp.copy(x.begin()+idx+1,x.end());
			x_ = tmp;

			copy_if(x_.begin(),x_.end(),tmp.begin(), );
		}*/
	
		bool operator<(const V& y) const{
     return x_ < y.x_; 
   }

	iterator begin(){
		return x_.begin();
	}

	iterator end(){
		return x_.end();
	}

	void insert(V val){
			x_.insert(x_.begin(),val);
	}
	

};

struct X_type: totally_ordered<X_type>, V_type<X_type, vector<double> >{
	private:
	vector<double> x_;

	public:
	void x(vector<double> t){
		for (size_type i=0; i < t.size(); ++i)
			x_.push_back(t[i]);
	}

	bool operator==(const X_type& x) const{
		/*if (x.size() != x_.size())
			return false;
		for(size_type i=0; i< x.size(); ++i){
        if (x[i] != x_[i])
				return false;
		return true;*/
		return x_ == x.x_; 
   }

   bool operator<(const X_type& x) const{
      return x_ < x.x_; 
   }

	vector<double>::iterator begin(){
		return x_.begin();
	}

	vector<double>::iterator end(){
		return x_.end();
	}
	
};

struct Y_type: totally_ordered<Y_type>, V_type<int, std::vector<int> >{

};


/*struct domain_value_type: domain_type<X>{
	size_type id;
	bool in_domain(X x){
		(void) x;
		return (rand()%100 > 90);
	}
};*/


struct random_forest_type;
typedef random_forest_type F;
typedef Manager<F> RandomForest;

struct policy_value_type: pvt<C> {
	private:

		std::vector<C> s_;

	public:	

		policy_value_type(){
		}

		void collect(){
			size_type num_ele = 10;
			
			size_type num_samples = 1+rand()%15;
			C tmp = C();

			for(size_type i = 0; i < num_samples; ++i){
				//C::value_type tmp;
				tmp.clear();
				for(size_type j=0; j < num_ele; ++j){
					tmp.push_back(rand()%400);//tmp.insert(tmp.end(),rand()%4000);
				}
				s_.push_back(tmp);//s_.insert(s_.end(),tmp);
			}
		}

		void clear(){
			s_.clear();
		}

		void stats(){
			cout << "Number in vector: " << s_.size() << endl;
			for(auto it = s_.begin(); it != s_.end(); ++it){
				for(auto itt = (*it).begin(); itt != (*it).end(); ++itt)
					cout << *itt << " ";
				cout << endl;
			}
			cout << endl;
		}

		bool has_met_limit(){
			return (rand()%100 < 10);
		}
	
		std::vector<C>& samples(){
			return s_;
		}

};

//template <typename X, typename Y>
struct Regression_type{
	private:
		vector<double> w_;
		bool has_fit_;

	public:
		Regression_type():w_(),has_fit_(false){
		}

		void fit(X_type& x,Y_type& y){
			(void) x;
			(void) y;
			cout << "Fitting parameters to training examples";
			w_.clear();
			w_.push_back(rand()%100);
			w_.push_back(rand()%100);
			w_.push_back(rand()%100);
			w_.push_back(rand()%100);
			has_fit_ = true;
		}

		Y predict(X_type& x){
			(void) x;
			//do some calculation 
			Y_type y_hat;
			for (size_type i=0; i < x.size(); ++i)
				y_hat.insert(rand()%1000);
			return y_hat;
		}

		double score(X_type x,Y_type y){
			if (!has_fit_)
				return 0.0;
				
			auto y_hat = predict(x);
			assert(y_hat.size()==y.size());

			auto it2 = y_hat.begin();
			double loss_val = 0.0;
			for (auto it1 = y.begin(); it1 < y.end(); ++it1, ++it2){
				loss_val += pow((*it1)-(*it2),2);
			}
			return loss_val;
		}

		void set_params(vector<double> w){
			w_.clear();
			w_.insert(w_.begin(),w.begin(),w.end());
		}

		vector<double> get_params(){
			return w_;
		}
		
		void operator=(Regression_type r){
			set_params(r.w_);
			r.has_fit_ = has_fit_;
		}

		bool operator==(Regression_type r){
			return (w_==r.w_) && (has_fit_==r.has_fit_);
		}

		bool operator!=(Regression_type r){
			return !((*this)==r);
		}
};

/*template <typename Q, typename X, typename Y>
struct LearningModelType {
	private:
		typedef Q<X,Y> P;
		P p_;
		
	public:

		LearningModelType(P p): p_(p){};

		void fit(X& x,Y& y){
			p_.fit(x,y);
		}
	
		Y predict(X& x){
			return p_.predict(x);
		}


		double score(X x,Y y)){
			return p_.score(x,y);
		}

		void set_params(P p){
			p_.set_params(p);
		}

		P get_params(){
			return p_.get_params();
		}

};*/


/**Timers**/
CS207::Clock start_time(){
	CS207::Clock t;
	return t;
}

void end_time(CS207::Clock t){
	cout << "Time: " << chrono::duration_cast<chrono::nanoseconds>(t.elapsed()).count() << '\n';
}

void visual_output(SamplerType Sa,string doing){
	(void) Sa;
	cout << "I just did something " << doing << endl;
	cout<<endl;
}

int main(){
	
	CS207::Clock t;	

	SamplerType Samp;
	t = start_time();	
	visual_output(Samp,"initiated variable with samples");
	SamplerType::Policy p1 = Samp.create_policy(25,60);
	SamplerType::Policy p2 = Samp.create_policy(50,60);
	visual_output(Samp,"created 2 policies");
	Samp.stats();

	Samp.start_collections();
	Samp.stats();
	visual_output(Samp,"initiated all collections");	

	p2.delete_samples();
	visual_output(Samp,"deleted all samples in one policy");
	Samp.stats();

	Manager<Regression_type,Regression_type> mgr;
	Regression_type r1;

	mgr.add_model(r1,r1);
	cout << "Number of models:" <<mgr.num_models() << endl;
	/*
	vector<double> vt;
	vt.push_back(5.5);
 	vt.push_back(5.7);
 	vt.push_back(23);
 	vt.push_back(26.5);
 	vt.push_back(1.5);

	regression_type rt1 {0,vt,10};

	rt1.update_w(0,5);
	rt1.update_w(1,6.6);
	rt1.update_w(2,0.25);
	rt1.update_w(3,1.25);
	

	auto m1 = r1.add_model(rt1);//causes segmentation fault
	(void) m1;
	
	regression_type rt2{1,vt,5};

	rt2.update_w(0,7);
	rt2.update_w(1,7.6);
	rt2.update_w(2,9.25);
	
	auto m2 = r1.add_model(rt2);

	cout << "Currently have "<< r1.num_models() << " loaded models" <<endl <<endl;

	SelectorType s;

	X x1;
	Y y1;

	s.train(r1, m1, x1, y1);
	Y y_hat = s.predict(r1, m1, x1);
	y_hat.add_element(0,1.5);
	double loss_overall = s.calculate_loss(r1,m1, y1,y_hat);

	cout << "Model Loss is"<< loss_overall << " units" <<endl <<endl;

	auto best_m = s.best_model(r1, x1, x1,y1,y1);
	cout << "Overall loss is " << loss_overall;
	(void) best_m;
	(void) m2; */
}










