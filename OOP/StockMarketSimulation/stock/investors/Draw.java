package investors;

import java.util.concurrent.ThreadLocalRandom;

/**
 * Class used to draw pseudo-random numbers form lower to upper (including ends
 * of the interval).
 */
public class Draw {
    public static int draw(int lower, int upper) {
        return ThreadLocalRandom.current().nextInt(lower, upper + 1);
    }
}