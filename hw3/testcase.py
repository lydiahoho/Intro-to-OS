import random

# Generate 1,000,000 different integers
num = 100000
unique_integers = random.sample(range(-1000000, 1000000), num )

# Convert the list to a space-separated string
result_string = ' '.join(map(str, unique_integers))

# Write the string to a file named input.txt
with open('input.txt', 'w') as file:
    file.write(str(num) + '\n')
    file.write(result_string)
