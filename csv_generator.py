#!/bin/python3

import csv
import random
import string
import sys

def generate_csv(file_name, num_columns, num_rows):
    """
    Generates a CSV file with random data.

    :param file_name: Name of the CSV file to create.
    :param num_columns: Number of columns in the CSV file.
    :param num_rows: Number of rows in the CSV file.
    """
    column_types = ['integer', 'float', 'text']
    column_headers = [f"column{i+1}" for i in range(num_columns)]
    column_generators = []

    for i in range(num_columns):
        column_type = random.choice(column_types)
        column_headers[i] += f"_{column_type}"
        if column_type == 'integer':
            column_generators.append(lambda: random.randint(-1000000, 1000000))
        elif column_type == 'float':
            column_generators.append(lambda: random.uniform(sys.float_info.min, sys.float_info.max))
        elif column_type == 'text':
            column_generators.append(lambda: ''.join(random.choices(string.ascii_letters, k=random.randint(5, 20))))

    with open(file_name, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(column_headers)
        for _ in range(num_rows):
            row = [gen() for gen in column_generators]
            writer.writerow(row)

if __name__ == "__main__":
    # Example usage
    file_name = "random_data.csv"
    num_columns = 5
    num_rows = 10000
    generate_csv(file_name, num_columns, num_rows)
