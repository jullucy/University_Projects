package publicTransport;

import static events.Event.convertTime;

/**
 * Stop is a class containing stops. Stops can be used to any vehicle,
 * because they only needs a name and information about passengers on them.
 */
public class Stop {
    private final String name;
    private final Passenger[] passengers;
    private int numberOfPassengers;

    /**
     * @param capacity is the full capacity of the stop, which will not be
     * changed during simulation.
     */
    public Stop(String name, int capacity) {
        this.name = name;
        this.numberOfPassengers = 0;
        this.passengers = new Passenger[capacity];
    }

    public int getNumberOfPassengers() {
        return numberOfPassengers;
    }

    public int getCapacity() {
        return passengers.length;
    }

    @Override
    public String toString() {
        return name;
    }

    public void newPassenger(Passenger passenger) {
        this.passengers[numberOfPassengers] = passenger;
        this.numberOfPassengers++;
    }

    /**
     * Used after every day of simulation. Adds awaiting time of passengers
     * which were waiting on the stop and didn't have a chance to get on the tram
     * at the end of the simulation.
     * Doesn't clear the passengers array, because it is sufficient to only
     * change numberOfPassengers value.
     */
    public void endOfDay(Simulation simulation) {

        for (int i = 0; i < numberOfPassengers; i++) {
            simulation.addWaitingTime(1440 - passengers[i].getStartTime());
            simulation.addHowManyWaited(1);
        }

        numberOfPassengers = 0;
    }

    /**
     * Method used to check if passengers can get on the vehicle
     * @param vehicle the Vehicle object into which passengers will board.
     * @param day Day of the simulation.
     * @param time Time on which vehicle arrived at the stop.
     * @return a sum of waiting time of all passengers who boarded the vehicle.
     */
    public int checkIfGetOnTram(Vehicle vehicle, int day, int time) {
        int howLongWaited = 0;

        for (int i = 0; i < numberOfPassengers; i++) {
            if (vehicle.isNotLoop() && vehicle.getNumberOfPassengers() < vehicle.getCapacity()) {
                howLongWaited += getOnTram(vehicle, day, time);
                i--;
            }
            else {
                i = numberOfPassengers;
            }
        }

        return howLongWaited;
    }

    /**
     * Helper method used when passenger can get on the tram.
     * @return how long (in minutes) passenger was waiting on the stop.
     */
    public int getOnTram(Vehicle vehicle, int day, int time) {
        this.numberOfPassengers--;
        vehicle.newPassenger(passengers[numberOfPassengers]);

        passengers[numberOfPassengers].setDestination(vehicle.destination());
        System.out.print("day " + day + ", " + convertTime(time) + ": ");
        String answer = "Passenger ";
        answer += passengers[numberOfPassengers].getNumber();
        answer += " got into the ";
        answer += vehicle.toString();
        answer += " with intention to get to ";
        answer += passengers[numberOfPassengers].getDestination().toString();
        answer += " stop.";
        System.out.println(answer);

        return time - passengers[numberOfPassengers].getStartTime();
    }
}
