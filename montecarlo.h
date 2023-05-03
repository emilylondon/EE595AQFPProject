#include <iostream>
#include <random>
#include <map>

using namespace std;

class Montecarlo{
    public:
        Montecarlo(string fname, int s);
        void output_simulation_results();
        void output_temp_ber();
    private:
        normal_distribution<double> temp_distribute;
        vector<string> in_names;
        vector<double> bit_error_rates;
        vector<double> temps;
        map<double, vector<double>> temp_ber;
        string orig_name;
        int simulations;
        
};