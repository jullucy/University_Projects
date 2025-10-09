package market;

import investors.Investor;

import java.io.FileNotFoundException;
import java.util.Collections;

/**
 * Simulation class for running a market simulation with investors and shares.
 */
public class Simulation {
    private final MarketSystem system;

    public Simulation(String[] args) throws FileNotFoundException {
        if (Integer.parseInt(args[1]) < 0) {
            throw new IllegalArgumentException("Number of turns cannot be a negative number.");
        }
        system = new MarketSystem(args);
    }

    public MarketSystem getSystem() {
        return system;
    }

    public void runSimulation(int numberOfTurns) {
        System.out.println(numberOfTurns);
        if (numberOfTurns < 0) {
            throw new IllegalArgumentException("Number of turns cannot be a negative number.");
        }

        while (system.getTurn() < numberOfTurns) {
            // Shuffle investors to randomize their decision-making order.
            Collections.shuffle(system.getInvestors());

            // Each investor makes a decision based on the current market state.
            for (Investor investor : system.getInvestors()) {
                investor.makeDecision(system);
            }

            // Sort investors based on their identification number.
            system.getInvestors().sort(Investor::compareNumber);

            // Process each share in the market system.
            for (int i = 0; i < system.getSharesLength(); i++) {
                Share share = system.getShares(i);
                share.sort(); // Sort orders related to this share.
                share.complete(system.getTurn()); // Complete orders related to this share.
            }

            // Move to the next turn.
            system.increaseTurn();
        }

        // Output the state of each investor's wallet.
        for (int i = 0; i < system.getInvestors().size(); i++) {
            System.out.print(system.getInvestors().get(i).getCash());

            for (int j = 0; j < system.getSharesLength(); j++) {
                System.out.print(" " + system.getShares(j).getIdentifier() + ":"
                        + system.getInvestors().get(i).getNumberOfShares(j));
            }

            System.out.println();
        }
    }

    public static void main(String[] args) {
        try {
            Simulation simulation = new Simulation(args);
            int numberOfTurns = Integer.parseInt(args[1]);
            System.out.println(numberOfTurns);
            if (numberOfTurns < 0) {
                throw new IllegalArgumentException("Number of turns cannot be a negative number.");
            }
            simulation.runSimulation(numberOfTurns);
        } catch (FileNotFoundException e) {
            throw new IllegalArgumentException(e);
        }
    }
}