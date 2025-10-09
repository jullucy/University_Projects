package publicTransport;

import java.util.Objects;

import static events.Event.convertTime;
import static publicTransport.Draw.draw;

/**
 * Abstract class representing vehicles.
 * New subclasses can be added, also new methods can be added (but methods added
 * in this class will affect tram class too).
 */
public abstract class Vehicle {
    private final int sideNumber;
    private final Line Line;
    private boolean firstSide; // Is vehicle on the tras of first side.
    private int currentStop; // Stop on which tram will be on nearest event.
    private int time; // Time of the day in minutes, on which vehicle will be
    // on next stop.
    private int numberOfPassengers;
    private final Passenger[] passengers; // Array of passengers that are
    // currently on the vehicle, only valid to index 'numberOfPassengers - 1'.
    private int beginning;

    public Vehicle(int sideNumber, Line Line, int capacity) {
        this.sideNumber = sideNumber;
        this.Line = Line;
        this.currentStop = 0;
        this.numberOfPassengers = 0;
        this.passengers = new Passenger[capacity];
    }

    public Line getLine() {
        return Line;
    }

    public int getSideNumber() {
        return sideNumber;
    }

    public Stop getCurrentStop() {
        Line line = getLine();
        Route route = line.getRoute();
        return route.getStop(currentStop);
    }

    public int getTime() {
        return time;
    }

    public int getNumberOfPassengers() {
        return numberOfPassengers;
    }

    public int getCapacity() {
        return passengers.length;
    }

    /**
     * Checks if vehicle is currently on the loop
     * @return true if vehicle is not on the loop, false if it is.
     */
    boolean isNotLoop() {
        int length =  getLine().getLengthOfRoute();

        if (firstSide) {
            return currentStop != length - 1;
        }
        else {
            return currentStop != 0;
        }
    }

    /**
     * Changes current stop to the next stop on the route.
     */
    public void nextStop() {
        Route route = getLine().getRoute();
        int length =  getLine().getLengthOfRoute();

        if (firstSide) {
            if (currentStop == length - 1) {
                this.time += route.getTime(length - 1);
                firstSide = !firstSide;
            }
            else {
                this.time += route.getTime(currentStop);
                currentStop++;
            }
        }
        else {
            if (currentStop == 0) {
                this.time += route.getTime(length - 1);
                firstSide = !firstSide;
            }
            else {
                this.time += route.getTime(currentStop - 1);
                currentStop--;
            }
        }
    }

    /**
     * Method used to start vehicles at the beginning of the day.
     * @param time time on which vehicle will start its daily route.
     * @param firstSide information about from which loop vehicle will start
     * its route.
     */
    public void start(int time, boolean firstSide) {
        if (firstSide) {
            this.currentStop = 0;
            this.beginning = 0;
        }
        else {
            int length = getLine().getLengthOfRoute();
            this.currentStop = length - 1;
            this.beginning = length - 1;
        }

        this.time = time;
        this.firstSide = firstSide;
    }

    /**
     * Used after every day of simulation. Doesn't clear the passengers array,
     * because it is sufficient to only change numberOfPassengers value.
     */
    public int endOfDay() {
        int answer = numberOfPassengers;
        numberOfPassengers = 0;

        return answer;
    }

    public void newPassenger(Passenger passenger) {
        this.passengers[numberOfPassengers] = passenger;
        this.numberOfPassengers++;
    }

    /**
     * Chooses pseudo-randomly a stop on the upcoming route of the vehicle.
     * @return drawn stop.
     */
    public Stop destination() {
        Line line = getLine();
        Route route = line.getRoute();
        int length = route.length();

        if (firstSide) {
            return route.getStop(draw(currentStop + 1, length - 1));
        }

        else {
            return route.getStop(draw(0, currentStop - 1));
        }
    }

    /**
     * Checks if any passenger wants and can get out of the vehicle.
     * Simulate getting out of vehicle if it is possible.
     * @param stop is a stop on which the vehicle has stopped.
     * @param day a day of the simulation.
     * @param time time on which vehicle arrived at the stop.
     * @return how many passengers completed their journey on this stop.
     */
    public int canGetOut(Stop stop, int day, int time) {
        int howManyOut = 0;
        for (int i = 0; i < numberOfPassengers; i++) {
            // Checks if it is possible to get out on this stop (check if the
            // number of passengers on the stop isn't maximum already).
            if (stop.getNumberOfPassengers() < stop.getCapacity()) {
                // Checks if this stop is the destination of chosen passenger.
                if (Objects.equals(passengers[i].getDestination().toString(), stop.toString())) {
                    getOut(stop, day, time, i);
                    howManyOut++;
                    i--;
                }

            }
            else {
                i = numberOfPassengers;
            }
        }
        return howManyOut;
    }

    /**
     * Simulate passenger under index 'i' getting out of the vehicle.
     */
    private void getOut(Stop stop, int day, int time, int i) {
        System.out.print("day " + day + ", " + convertTime(time) + ": ");
        System.out.println("Passenger " + passengers[i].getNumber() +
                " get out of " + this + "on " + stop + " stop.");

        stop.newPassenger(passengers[i]);
        this.numberOfPassengers--;
        passengers[i].setStartTime(time);

        for (int j = i; j < numberOfPassengers; j++) {
            passengers[j] = passengers[j + 1];
        }
    }

    public boolean isBeginning() {
        return beginning == currentStop;
    }

}
