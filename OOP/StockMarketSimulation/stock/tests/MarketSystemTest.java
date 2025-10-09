package tests;
import market.MarketSystem;
import org.junit.jupiter.api.Test;

import java.io.FileNotFoundException;

import static org.junit.jupiter.api.Assertions.*;

public class MarketSystemTest {

    @Test
    public void testInitialization() throws FileNotFoundException {
        String[] args = {"src\\tests\\testfiles\\test1.txt", "10"};
        MarketSystem system = new MarketSystem(args);

        assertNotNull(system.getInvestors(), "Investors should be initialized");
        assertNotNull(system.getShares(0), "Shares should be initialized");
    }

    @Test
    public void testGetTurn() throws FileNotFoundException {
        String[] args = {"src\\tests\\testfiles\\test1.txt", "10"};
        MarketSystem system = new MarketSystem(args);

        assertEquals(0, system.getTurn(), "Initial turn should be 0");
    }

    @Test
    public void testIncreaseTurn() throws FileNotFoundException {
        String[] args = {"src\\tests\\testfiles\\test1.txt", "10"};
        MarketSystem system = new MarketSystem(args);
        system.increaseTurn();

        assertEquals(1, system.getTurn(), "Turn should be incremented");
    }
}