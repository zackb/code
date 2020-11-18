import { Person } from './person.js';

const Car = function(make, speed) {
    this.make = make;
    this.speed = speed;
};

Car.prototype.accelerate = function() {
    this.speed += 10;
    console.log(`New speed: ${this.speed}`);
};

Car.prototype.brake = function() {
    this.speed -= 5;
    console.log(`New speed: ${this.speed}`);
};


const Truck = function(make, speed, color, driver) {
    Car.call(this, make, speed);
    this.color = color;
    this.driver = driver;
}

// WTF
Truck.prototype = Object.create(Car.prototype);
Truck.prototype.constructor = Car;

export const FordF150 = new Truck('Ford', 55, 'red');