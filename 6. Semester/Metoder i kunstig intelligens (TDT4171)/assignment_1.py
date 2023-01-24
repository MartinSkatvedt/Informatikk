from random import randint, choices
import statistics

# Author: Martin Skatvedt 20/01/2023

# Exercise 2
# c)

SLOT_SYMBOLS = ["BAR", "BELL", "LEMON", "CHERRY"] 

payouts = { 
    ("BAR", "BAR", "BAR"): 20,
    ("BELL", "BELL", "BELL"): 15,
    ("LEMON", "LEMON", "LEMON"): 5,
    ("CHERRY", "CHERRY", "CHERRY"): 3,
}

def simulate_slot() -> int: 
    wheels = tuple(choices(SLOT_SYMBOLS, k=3))
    if wheels in payouts:
        return payouts[wheels]
    else:
        if wheels[0] == "CHERRY" and wheels[1] == "CHERRY":
            return 2
        if wheels[0] == "CHERRY":
            return 1
        return 0

def ex_2c(n_iterations: int):
    print(f'\nStarting Exercise 2c, with {n_iterations} simulations\n')
    
    all_games_played = []

    for _ in range(0, n_iterations):
        coins = 10
        games_played = 0
        while True:
            coins -= 1
            coins += simulate_slot()
            games_played += 1
            if (coins == 0):
                all_games_played.append(games_played)
                break


    mean = statistics.mean(all_games_played)
    median = statistics.median(all_games_played)
    print(f'With {n_iterations} games played, the mean is {round(mean, 2)} games, and median is {round(median, 2)} games.')

# Exercise 3
# Part 1
# a)


def simulate_birthday_event(N: int, max_iterations: int) -> float:
    iteration = 0
    iterations_with_same_birthday = 0

    while iteration < max_iterations:
        birthdays = [0] * N
        for index in range(0, N):
            birthdays[index] = randint(1, 365)

        if len(birthdays) != len(set(birthdays)):
            iterations_with_same_birthday += 1
        iteration += 1
    
    return (iterations_with_same_birthday/max_iterations) * 100

# b)

def ex_3_part_1(n_iterations: int):
    print(f'\nExercise 3\nStarting part 1, with {n_iterations} simulations\n')
    
    min_N_needed = float("inf")
    for N in range(10, 51):
        p = simulate_birthday_event(N, n_iterations)
        if p > 50.0 and min_N_needed > 51:
            min_N_needed = N
        print(f'{N} people has a {round(p, 2)}% to have the event')

    print(f'{min_N_needed} is the smallest group needed, to have at least a 50% chance of the event happening')



# Part 2
def simulate_birthday_assignment() -> int:
        group = []
        while True:
            group.append(randint(1, 365))

            if len(set(group)) == 365:
                return len(group)

def ex_3_part_2(n_iterations):
    print(f'\nStarting part 2, with {n_iterations} simulations\n')

    group_sizes = [0] * n_iterations

    for iteration in range(0, n_iterations):
        group_sizes[iteration] = simulate_birthday_assignment()

    average_group_size = sum(group_sizes) / n_iterations 
    print(f'Average group size is {round(average_group_size, 2)} with {n_iterations} samples')


def main():
    ex_2c(10_000)
    ex_3_part_1(10_000)
    ex_3_part_2(100)

if __name__ == "__main__":
    main()
