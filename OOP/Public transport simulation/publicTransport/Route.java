package publicTransport;

/**
 * Class representing route of the vehicle, this is its stops and time needed to
 * transport between every stop.
 * Can be used for different type of vehicles.
 */
public class Route {
    private final Stop[] stops;
    private final int[] time;
    public Route(Stop[] stops, int[] time) {
        this.stops = stops;
        this.time = time;
    }

    @Override
    public String toString() {
        StringBuilder answer = new StringBuilder();

        for (int i = 0; i < stops.length; i++) {
            answer.append(" ").append(stops[i]).append(" ");
            answer.append(time[i]);
        }

        return answer.toString();
    }

    /**
     * Helper function needed while counting the gaps between vehicles of the same line.
     * @return full time of the route, from both sides, including waiting on the loops.
     */
    public int GetFullTime() {
        int fullTime = 0;

        for (int j : time) {
            fullTime += j;
        }

        fullTime *= 2;
        return fullTime;
    }

    public Stop getStop(int i) {
        return stops[i];
    }

    public int getTime(int i) {
        return time[i];
    }

    public int length() {
        return stops.length;
    }
}