import argparse
import subprocess
import math
import numpy as np 

BOLTZMANN = 1.38064852E-23
T_c = 7.2
beta_c = 0.5
EV = 1.6021766208e-19

def mod_Ic(new_temp: float, old_ic: float) -> float:
    """
    Outputs the newly calculated critical current given the new temperature and the old critical current value.
    
    Args: 
        new_temp (float): The new temperature to set in Kelvin
        old_ic  (float): the old critical current value
    """
    Ic_0 = old_ic  # A
    Jc_0 = 1e6  # A/m^2
    A = 1e-12  # m^2 (cross-sectional area)
    T_c = 4.2  # K
    B = 1.0  # material-dependent parameter
    k_B = 1.38e-23  # J/K
    T_new = new_temp  # K

    # Compute new critical current density
    #Jc_new = Jc_0 * (T_new/T_c) * ((1 - abs((T_new/T_c)**2)**(3/2))) * np.exp(-B * abs((1 - (T_new/T_c)**2)**(1/2)))

    # Compute new critical current
    Ic_new = old_ic * 4.2/new_temp

    return Ic_new
    """
    #basically if the new_temp > critical current set to zero, should fail always anyways 
    if root_val < 0:
        root_val = 0
    mid = math.pi / 2 * math.sqrt(root_val)
    cos_t = abs(math.cos(mid))
    sin_t = math.sqrt(1+ beta_c**2 * math.sin(mid)**2)
    exp_t = math.exp(-delta_0/(BOLTZMANN*new_temp))
    try: 
        temp_ = old_ic * cos_t / sin_t * exp_t
    except: 
        temp_ = old_ic * 0.0001
    """
    """
    del0_ = 1.76 * BOLTZMANN * T_c
    del_ = del0_ * math.sqrt(abs(math.cos((math.pi / 2) * (new_temp / T_c) * (new_temp/ T_c))))
    r_n = ((math.pi * del_) / (2 * EV * old_ic)) * math.tanh(del_ / (2 * BOLTZMANN * new_temp))
    mc_ = 2*math.pi*old_ic/(2.068*pow(10,-15))
    ceta_ = math.pi/2*math.sqrt(abs(1-new_temp/T_c))
    temp_ = old_ic*abs(math.cos(ceta_)) / math.sqrt(1+mc_*mc_*math.sin(ceta_)*math.sin(ceta_))
    """
    #return temp_


def edit_file(input_file: str, new_temp: float, index: int) -> str:
    """
    Edits a .cir file by updating the temperature parameter and saves the edited file with an index.

    Args:
        input_file (str): The path to the input file to edit.
        new_temp (int): The new temperature to set in Kelvin.
        index (int): The index to add to the output file name.

    Returns:
        str: The path to the output file.

    """
    

    output_file = input_file.split(".")[0] + "_" + str(index) + "." + input_file.split(".")[1]

    subprocess.run(['python3', 'noise_insert.py', input_file, '-o', output_file, '-t', str(new_temp)])

    # read the file
    with open(output_file, 'r') as f:
        lines = f.readlines()

    # modify the line that starts with ".model jjmod"
    for i, line in enumerate(lines):
        if line.startswith('.model jjmod'):
            old_line = line
            # extract the old Icrit value from the line
            str_old_ic = line.split(', Icrit=')[1].split('A')[0]
            old_ic = float(line.split(', Icrit=')[1].split('A')[0])
            # call the mod_Ic function to get the new Icrit value
            new_ic = mod_Ic(new_temp, old_ic) # pass a new temperature of 5 K
            print(old_ic)
            print(new_ic)
            # replace the old Icrit value with the new value in the line
            new_line = old_line.replace(f', Icrit={str_old_ic}', f', Icrit={new_ic}')
            print(new_line)
            lines[i] = new_line

    # write the modified file back to disk
    with open(output_file, 'w') as f:
        f.writelines(lines)

    return output_file


def run_josim(file: str) -> None:
    """
    Runs the josim command-line tool on a .cir file and saves the output to a CSV file.

    Args:
        file (str): The path to the .cir file to run.

    """
    output_csv = file.split(".")[0] + ".csv"
    subprocess.run(['josim', file, '-o', output_csv], check=True)


if __name__ == "__main__":
    # Define the command-line arguments using argparse
    parser = argparse.ArgumentParser(description='Edit .cir file with updated temperature.')
    parser.add_argument('input_file', type=str, help='path to input file')
    parser.add_argument('temp', type=float, help='new temperature in Kelvin')
    parser.add_argument('index', type=int, help='index for monte carlo simulation')
    args = parser.parse_args()

    # Edit the input file with the new temperature and save to a new file with an index
    new_cir_file = edit_file(args.input_file, args.temp, args.index)

    # Run josim on the new file and save the output to a CSV file
    run_josim(new_cir_file)