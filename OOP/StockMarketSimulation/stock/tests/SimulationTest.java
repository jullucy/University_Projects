package tests;

import investors.Investor;
import market.MarketSystem;
import market.Simulation;
import org.junit.jupiter.api.Test;

import java.io.FileNotFoundException;

import static org.junit.jupiter.api.Assertions.*;

public class SimulationTest {

    /**
     * Helper method to calculate the total cash held by all investors.
     */
    private int getTotalCash(MarketSystem system) {
        int ans = 0;
        for (int i = 0; i < system.getInvestors().size(); i++) {
            ans += system.getInvestors().get(i).getCash();
        }
        return ans;
    }

    /**
     * Helper method to calculate the total number of shares of a given type held by all investors.
     */
    private int getTotalShares(MarketSystem system, int shareIndex) {
        return system.getInvestors().stream()
                .mapToInt(investor -> investor.getNumberOfShares(shareIndex))
                .sum();
    }

    public void totalCashAndSharesRemainConstant(String[] args) throws FileNotFoundException {
        // Setup initial conditions and create the Simulation instance
        Simulation simulation = new Simulation(args);
        MarketSystem system = simulation.getSystem();

        // Capture initial total cash and shares
        int initialTotalCash = getTotalCash(system);
        int[] initialTotalShares = new int[system.getSharesLength()];
        for (int i = 0; i < system.getSharesLength(); i++) {
            initialTotalShares[i] = getTotalShares(system, i);
        }

        // Capture final total cash and shares
        int finalTotalCash = getTotalCash(system);
        int[] finalTotalShares = new int[system.getSharesLength()];
        for (int i = 0; i < system.getSharesLength(); i++) {
            finalTotalShares[i] = getTotalShares(system, i);
        }

        // Assert total cash remains constant
        assertEquals(initialTotalCash, finalTotalCash);

        // Assert total shares remain constant
        for (int i = 0; i < system.getSharesLength(); i++) {
            assertEquals(initialTotalShares[i], finalTotalShares[i]);
        }
    }

    @Test
    public void testTotalCashAndSharesRemainConstant() throws FileNotFoundException {
        String[] args = {"src\\tests\\testfiles\\moodle.txt", "100000"};
        totalCashAndSharesRemainConstant(args);
    }

    @Test
    public void testCashShares1() throws  FileNotFoundException{
        String[] args = {"src\\tests\\testfiles\\testCashShare1", "100000"};
        totalCashAndSharesRemainConstant(args);
    }

    @Test
    public void testCashShares2() throws  FileNotFoundException{
        String[] args = {"src\\tests\\testfiles\\testCashShare2", "1000"};
        totalCashAndSharesRemainConstant(args);
    }

    @Test
    public void testCashShares3() throws  FileNotFoundException{
        String[] args = {"src\\tests\\testfiles\\testCashShare3", "100077"};
        totalCashAndSharesRemainConstant(args);
    }

    @Test
    public void testCashShares4() throws  FileNotFoundException{
        String[] args = {"src\\tests\\testfiles\\testCashShare4", "52000"};
        totalCashAndSharesRemainConstant(args);
    }

    /**
     * If in the simulation are only SMA investors they shouldn't do anything.
     */
    @Test
    public void onlySma() throws  FileNotFoundException{
        String[] args = {"src\\tests\\testfiles\\onlySma", "0"};
        Simulation simulation = new Simulation(args);
        MarketSystem system = simulation.getSystem();
        int numberOfInvestors = system.getInvestors().size();
        int numberOfShares = system.getSharesLength();

        // Record initial state of investors
        int[] initialCash = new int[numberOfInvestors];
        int[][] initialShares = new int[numberOfInvestors][numberOfShares];
        for (int i = 0; i < numberOfInvestors; i++) {
            Investor investor = system.getInvestors().get(i);
            initialCash[i] = investor.getCash();
            for (int j = 0; j < numberOfShares; j++) {
                initialShares[i][j] = investor.getNumberOfShares(j);
            }
        }

        args = new String[]{"src\\tests\\testfiles\\onlySma", "10000000"};
        simulation = new Simulation(args);
        system = simulation.getSystem();

        // Record final state of investors
        int[] finalCash = new int[numberOfInvestors];
        int[][] finalShares = new int[numberOfInvestors][numberOfShares];
        for (int i = 0; i < numberOfInvestors; i++) {
            Investor investor = system.getInvestors().get(i);
            finalCash[i] = investor.getCash();
            for (int j = 0; j < numberOfShares; j++) {
                finalShares[i][j] = investor.getNumberOfShares(j);
            }
        }

        // Verify initial and final state are identical
        assertArrayEquals(initialCash, finalCash, "Cash values should remain unchanged.");
        for (int i = 0; i < numberOfInvestors; i++) {
            assertArrayEquals(initialShares[i], finalShares[i], "Share values should remain unchanged for investor " + i);
        }
    }

    @Test
    public void testMissingInvestors() {
        String[] args = {"src\\tests\\testfiles\\invalid_input_missing_investors.txt", "10"};
        assertThrows(IllegalArgumentException.class, () -> new MarketSystem(args));
    }

    @Test
    public void testInvalidSharePrices() {
        String[] args = {"src\\tests\\testfiles\\invalid_input_invalid_share_prices.txt", "10"};
        assertThrows(IllegalArgumentException.class, () -> new MarketSystem(args));
    }

    @Test
    public void testIncorrectPortfolioFormat() {
        String[] args = {"src\\tests\\testfiles\\invalid_input_incorrect_portfolio_format.txt", "10"};
        assertThrows(IllegalArgumentException.class, () -> new MarketSystem(args));
    }

    @Test
    public void testInvalidTurnNumber() {
        String[] args = {"src\\tests\\testfiles\\moodle.txt", "-5"};
        assertThrows(IllegalArgumentException.class, () -> new Simulation(args));
    }


}
