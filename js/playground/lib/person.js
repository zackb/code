export class Person {
    constructor(firstName, birthYear) {
        this.firstName = firstName;
        this.birthYear = birthYear;
    }
    asString() {
        return `${this.firstName} ${this.birthYear}`;
    }

    get fullName() {
        return `${this.firstName} lastName`;
    }
}