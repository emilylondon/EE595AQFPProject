import argparse
import subprocess


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
    
    print(input_file)
    print(output_file)

    print(new_temp)

    subprocess.run(['python3', 'noise_insert.py', input_file, '-o', output_file, '-t', str(new_temp)])

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
