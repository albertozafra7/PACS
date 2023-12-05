import re
import argparse

def process_input(input_file_path, output_file_path):
	# Regular expressions for matching the input lines
	user_pattern = r"(\d+\.\d+)user "
	system_pattern = r"(\d+\.\d+)system "
	elapsed_pattern_min = r"(\d+):"
	elapsed_pattern_sec = r"(\d+\.\d+)elapsed"
	cpu_pattern = r"(\d+)%CPU"

	try:
		with open(input_file_path, "r") as input_file, open(output_file_path, "w") as output_file:
			for line in input_file:
				user_match = re.search(user_pattern, line)
				system_match = re.search(system_pattern, line)
				elapsed_min_match = re.search(elapsed_pattern_min, line)
				elapsed_sec_match = re.search(elapsed_pattern_sec, line)
				cpu_match = re.search(cpu_pattern, line)

				if user_match and system_match and elapsed_min_match and elapsed_sec_match and cpu_match:
					user_time = user_match.group(1)
					system_time = system_match.group(1)
					elapsed_time = float(elapsed_min_match.group(1))*60 + float(elapsed_sec_match.group(1))
					cpu_percentage = cpu_match.group(1)

					# Format the extracted data as desired
					formatted_line = f"{user_time},{system_time},{elapsed_time},{cpu_percentage}\n"

					# Write the formatted line to the output file
					output_file.write(formatted_line)
	except Exception as e:
		print(f"An error occurred: {e}")

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Process input file and save results to output file")
	parser.add_argument("input_file", help="Path to the input file")
	parser.add_argument("output_file", help="Path to the output file")
	args = parser.parse_args()

	process_input(args.input_file, args.output_file)

