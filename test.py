from yoyo import print # type: ignore
from test2 import add


print('YOYO Python!')

class Person:
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def greet(self, other:Person):
        print(f"Hi {other.name}! My name is {self.name} and I am {self.age} years old!")

jordan = Person('Jordan', 24)
evelyn = Person('Evelyn', 21)

jordan.greet(evelyn)
evelyn.greet(jordan)

