#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>
#include "rapidcsv.h"
#include "ber.h"
#include "montecarlo.h"

using namespace std;

//gaussian distribution for amount of temp difference from ideal (4.2 K)
//inputs: total number of samples
//        file name of the circuit to vary  

/**
 * Runs Montecarlo simulation of a set size from an input circuit to determine bit error rate 
 * @param fname Input circuit file name 
 * @param s Simulation size 
**/
Montecarlo::Montecarlo(string fname, int s){
  orig_name = fname;
  simulations = s;
  normal_distribution<double> distribute(0, 2.95);
  temp_distribute = distribute;
  default_random_engine generator;

  //generate S number of Josim simulations at temperature from normal distribution 
  for (int i = 0; i<simulations; i++){
    temps.push_back(4.2+abs(temp_distribute(generator)));
    //run python script with the name being something like "orig_name+i.csv"
    string cmd_str = "python3 josim_temp_simulator.py " + orig_name + " " + to_string(temps[i]) + " " + to_string(i);
    system(cmd_str.c_str());
    in_names.push_back(orig_name.substr(0, orig_name.length()-4) + "_" +to_string(i) + ".csv");
    cout << in_names[i] << endl;
    BER b(in_names[i]); //finds bit error rate for that file 
    bit_error_rates.push_back(b.find_ber());
    //set temperature error rate map values
    if (temp_ber.count(temps[i]) > 0){
      temp_ber[temps[i]][0] += bit_error_rates[i];
      temp_ber[temps[i]][1] += 1;
    }
    else {
      vector<double> vals;
      vals.push_back(bit_error_rates[i]);
      vals.push_back(1);
      temp_ber[temps[i]] = vals;
    }
  }
}

/**
 * Outputs total simulation results to a csv file. Best if used for 
 * scatter plot of overall simulation results.
**/
void Montecarlo::output_simulation_results(){
   fstream fout;
   string fout_name = orig_name.substr(0, orig_name.length()-4) + "_scatter.csv";
   string rm_name = "rm " + fout_name;
   system(rm_name.c_str());
   fout.open(fout_name, ios::out | ios::app);
   fout << "item,temp,ber" << "\n";
   for (int i = 0; i < simulations; i++){
      fout << i << "," << temps[i] << "," << bit_error_rates[i] << "\n";
   }
}

/**
 * Outputs ber vs temp results to a csv file. Best if used for 
 * the plot of BER vs temp 
**/
void Montecarlo::output_temp_ber(){
   fstream fout;
   string fout_name = orig_name.substr(0, orig_name.length()-4) + "_temp_ber.csv";
   string rm_name = "rm " + fout_name;
   system(rm_name.c_str());
   fout.open(fout_name, ios::out | ios::app);
   fout << "temp,ber" << "\n";
   for (auto i = temp_ber.begin(); i != temp_ber.end(); i++){
      fout << i->first << "," << i->second[0]/i->second[1] << "\n";
   }
}

/**
 * Destructor function for Montecarlo class. Deletes the created cir and csv files from the
 * simulation.
**/

Montecarlo::~Montecarlo(){
  for (int i = 0; i < in_names.size(); i++){
    string cir_file = "rm " + in_names[i].substr(0, in_names[i].length()-4)+".cir";
    string csv_file = "rm " + in_names[i];
    system(cir_file.c_str());
    system(csv_file.c_str());
  }
}


int main(){
  Montecarlo m("examples/ex_pi_DQFP_buffer_chan.cir", 100);
  m.output_simulation_results();
  m.output_temp_ber();
  return 0;
}