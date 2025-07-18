# This is a single-line comment in Python

def greet(name):
    """
    This is a multi-line docstring.
    It explains the function's purpose.
    """
    message = f"Hello, {name}!"
    print(message)
    return message

if __name__ == "__main__":
    user_name = "Whoami"
    greet(user_name)

    # Example of a list and a loop
    numbers = [1, 2, 3, 4, 5]
    for num in numbers:
        if num % 2 == 0:
            print(f"Even number: {num}")
        else:
            print(f"Odd number: {num}")

    my_float = 3.14159
    my_hex = 0xFF
