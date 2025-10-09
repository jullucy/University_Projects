package tests;

import investors.Investor;
import investors.RandomInvestor;
import orders.CompleteOrCancelOrder;
import orders.Order;
import market.Share;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Unit tests for the CompleteOrCancelOrder class.
 */
public class CompleteOrCancelOrderTest {

    private Investor investor;
    private Share share;

    /**
     * Sets up the test environment.
     */
    @BeforeEach
    public void setUp() {
        investor = new RandomInvestor(1, 0);
        investor.setCash(10000);
        investor.setShares(new int[]{100});
        share = new Share("AAPL", 150, 0);
    }

    /**
     * Tests the attributes of the CompleteOrCancelOrder.
     */
    @Test
    public void testCompleteOrCancelOrderAttributes() {
        CompleteOrCancelOrder order = new CompleteOrCancelOrder(investor, 0, 0, 10, 150, 1);
        assertEquals(1, order.getTurn(), "Turn should be 1.");
        assertEquals(2, order.getType(), "Order type should be 2 (CompleteOrCancel).");
    }

    /**
     * Tests the copy constructor of the CompleteOrCancelOrder.
     */
    @Test
    public void testCopyConstructor() {
        CompleteOrCancelOrder original = new CompleteOrCancelOrder(investor, 0, 0, 10, 150, 1);
        CompleteOrCancelOrder copy = new CompleteOrCancelOrder(original);

        assertEquals(original.getTurn(), copy.getTurn(), "Turn should be equal to the original order's turn.");
    }

    /**
     * Tests if the order is removed when the number of shares exceeds the investor's shares.
     */
    @Test
    public void testRealizeSale_RemovesOrderIfSharesExceed() {
        Order order = new CompleteOrCancelOrder(investor, 1, 0, 200, 150, 1); // More shares than investor has
        ArrayList<Order> saleOrders = new ArrayList<>();
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        saleOrders.add(order);

        order.realizeSale(1, saleOrders, purchaseOrders, 0, share);

        assertTrue(saleOrders.isEmpty(), "Order should be removed if shares exceed investor's shares.");
    }

    /**
     * Tests if the order is removed when the number of shares is zero.
     */
    @Test
    public void testRealizeSale_RemovesOrderIfSharesZero() {
        Order order = new CompleteOrCancelOrder(investor, 1, 0, 0, 150, 1); // Zero shares
        ArrayList<Order> saleOrders = new ArrayList<>();
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        saleOrders.add(order);

        order.realizeSale(1, saleOrders, purchaseOrders, 0, share);

        assertTrue(saleOrders.isEmpty(), "Order should be removed if shares are zero.");
    }

}
