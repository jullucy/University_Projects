package publicTransport;

/**
 * Abstract class representing lines of vehicles - trams ect.
 * New subclasses can be added, but this class cannot be changed
 * (it can only be extended).
 */
public abstract class Line {
    private final int number;
    private final int numberOfVehicles;
    private final int lengthOfRoute;
    private Route route;

    public Line(int number, int numberOfVehicles, int lengthOfRoute) {
        this.number = number;
        this.numberOfVehicles = numberOfVehicles;
        this.lengthOfRoute = lengthOfRoute;
    }
    public int getLengthOfRoute() {
        return lengthOfRoute;
    }

    public int getNumberOfVehicles() {
        return numberOfVehicles;
    }

    public int getNumber() {
        return number;
    }

    public void setRoute(Stop[] stops, int[] time) {
        this.route = new Route(stops, time);
    }

    public Route getRoute() {
        return route;
    }

    public int countGaps() {
        return route.GetFullTime() / numberOfVehicles;
    }

    @Override
    public String toString() {
        String answer;
        answer = Integer.toString(numberOfVehicles);
        answer += (" " + lengthOfRoute);
        answer += route.toString();
        return answer;
    }
}
