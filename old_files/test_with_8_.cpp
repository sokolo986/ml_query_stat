#include "Sampler.hpp"
//#include "Container.hpp"

using namespace std;
struct sample_value_type;
struct policy_value_type;

typedef sample_value_type S;
typedef policy_value_type P;
typedef unsigned size_type;
typedef void C;
typedef Sampler<P,S,C> SamplerType;
typedef SamplerType::Policy policy;

struct policy_value_type: pvt<C> {
	private:
		int x_;
		int y_;
	public:	
		policy_value_type(int x, int y):x_(x),y_(y){}
		policy_value_type(): x_(0),y_(0){}
		C collect(){for(int i=0; i<5;++i) cout <<"collecting";cout<<endl;}
		C stop_collect(){for(int i=0; i<5;++i) cout <<"stop";cout<<endl;}
		void mark_if_meet_constraints(){
			x_ = 15;
		}
		void enforce_policy(){
			x_ = 15;
			y_ = 12;
		}
		void print_val(){
			cout << "X:   " << x_ << "  Y:   " << y_;
			cout << endl << endl;
		}
	
};

struct sample_value_type{
	size_type id;
	string	user_name;
	string	address;
};

/**Timers**/
CS207::Clock start_time(){
	CS207::Clock t;
	return t;
}

void end_time(CS207::Clock t){
	cout << "Time: " << chrono::duration_cast<chrono::nanoseconds>(t.elapsed()).count() << '\n';
}

void visual_output(SamplerType Sa,string doing){
	cout << "I just " << doing << endl;
	Sa.print_stats();
	Sa.free_map();
	Sa.used_map();
	cout<<endl;
}

int main(){
	
	CS207::Clock t;	

	Sampler<P,S,void> Samp(15);
	t = start_time();	
	visual_output(Samp,"initiated variable with samples");
	Samp.clear();
	visual_output(Samp,"cleared the container");
	auto pol = Samp.create_policy();
	auto pol2 = Samp.create_policy();	
	visual_output(Samp,"created 2 policies");
	Samp.start_collection(pol);
	assert(Samp.get_id(pol)!=Samp.get_id(pol2));
	visual_output(Samp,"started collection");
	Samp.stop_collection(pol);
	visual_output(Samp,"stopped a running collection");
	policy_value_type pv(8,2);
	Samp.start_collection(pv);
	pv.print_val();
	visual_output(Samp,"started a policy with policy_value_type");
	Samp.stop_collections();
	visual_output(Samp,"just stopped all collections");
	auto pol4 = Samp.copy_policy(pol);
	visual_output(Samp,"copied another policy");
	Samp.create_policies(5);
	visual_output(Samp,"created 5 new policies");
	Samp.clear();
	Samp.create_policies(2);
	visual_output(Samp,"created cleared and created 2");
	Samp.policy_value(pol4)= pv;
	visual_output(Samp,"pv placed in policy 4");
	end_time(t);	
	
}










