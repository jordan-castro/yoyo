import {print} from 'yoyo';

class Person {
    constructor(name, age) {
        this.name = name;
        this.age = age;
    }

    greet(other) {
        print(`Hello ${other.name}! My name is ${this.name}. And I am ${this.age} years old.`);
    }
}

print("YOYO JS!");

let jordan = new Person("Jordan", 24);
let evelyn = new Person("Evelyn", 21);

jordan.greet(evelyn);
evelyn.greet(jordan);