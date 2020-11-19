// 'use strict';
import { FordF150 } from './lib/car.js';
import { Person } from './lib/person.js';
import { v4 as uuid } from 'uuid';

const obj = {
    id: 1,
    name: 'foo',
    date: new Date(),
    toString: function() { 
        return `ID: ${this.id} Name: ${this.name} Date: ${this.date}`
    },
};

(function() {
    setTimeout(() => {
        console.log(document.querySelector('p').textContent = 'Yo!');
    }, 3000);
})();

const zack = new Person('Zack', 1983);

FordF150.driver = zack;

FordF150.accelerate();

console.dir(FordF150);
console.log('hell0');
console.log(uuid());