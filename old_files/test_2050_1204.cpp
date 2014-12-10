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
		void print_val(){
			cout << "X:   " << x_ << "  Y:   " << y_;
			cout << endl << endl;
		}
		
	
};

struct sample_value_type{
	size_type id;
	string	user_name;
	string	address;
	void operator=(sample_value_type a){
		id = a.id;
		user_name = a.user_name;
		address = a.address;
	}
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
	Samp.create_policy();
	Samp.create_policy();
	Samp.create_policy();
	visual_output(Samp,"created 3 policy");
	Samp.start_collections();
	visual_output(Samp,"initiated all collections");
	Samp.stop_collections();
	visual_output(Samp,"stopped all collections");/*
	Samp.increase_num_policies(5);
	visual_output(Samp,"increased policy limits");
	Samp.create_policies(5);
	visual_output(Samp,"created new policies");
	Samp.policy(1);*/
	for( auto it = Samp.policies(); it!=Samp.policies_end(); ++it)
		cout << "policy -- ";
	cout << endl;		
	visual_output(Samp,"created an iterator to policies");
	cout << "max policies " << Samp.max_num_policies();
	cout << "num policies " << Samp.num_policies();
	cout << "num active policies " << Samp.num_active_policies();
	cout << "num inactive policies " << Samp.num_inactive_policies();
	cout << "unused ids " << Samp.total_unused_ids();
	auto pol = Samp.create_policy();
	Samp.start_collection(pol);
	visual_output(Samp,"output statistics and started collection for a new policy");
	Samp.stop_collection(pol);
	visual_output(Samp,"stopped policy");
	cout << "Valid?" << Samp.is_valid(pol);
	auto pol2 = Samp.create_policy();
	Samp.copy_policy(pol,pol2);
	visual_output(Samp,"marked and enforced and created policy");
	
	sample_value_type sv{1,"Sierra","23 Mulberry Lane"};
	Samp.add_sample(pol2, sv);
	visual_output(Samp,"added sample");

	Samp.delete_samples(pol2);
	visual_output(Samp,"added sample");
	end_time(t);	
	
}










