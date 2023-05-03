#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include <map>
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
  normal_distribution<double> distribute(4.2, 2.95);
  temp_distribute = distribute;
  default_random_engine generator;

  //generate S number of Josim simulations at temperature from normal distribution 
  for (int i = 0; i<simulations; i++){
    temps.push_back(temp_distribute(generator));
    //run python script with the name being something like "orig_name+i"
    in_names.push_back(orig_name+to_string(i));
    BER b(in_names[i]); //finds bit error rate for that file 
    bit_error_rates.push_back(b.find_ber());
    //set temperature error rate map values
    if (temp_ber.count(temps[i]) > 0){
      vector<double> vals;
      vals.push_back(bit_error_rates[i]);
      vals.push_back(1);
    }
    else {
      temp_ber[temps[i]][0] += bit_error_rates[i];
      temp_ber[temps[i]][1] += 1;
    }
  }
}

/**
 * Outputs total simulation results to a csv file. Best if used for 
 * scatter plot of overall simulation results.
**/
void Montecarlo::output_simulation_results(){
   fstream fout;
   string fout_name = orig_name.substr(0, orig_name.length()-3) + "_scatter.csv";
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
   string fout_name = orig_name.substr(0, orig_name.length()-3) + "_temp_ber.csv";
   fout.open(fout_name, ios::out | ios::app);
   fout << "temp,ber" << "\n";
   for (auto i = temp_ber.begin(); i != temp_ber.end(); i++){
      fout << i->first << "," << i->second[0]/i->second[1] << "\n";
   }
}
