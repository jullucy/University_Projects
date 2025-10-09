package tests;

import investors.Investor;
import investors.RandomInvestor;
import orders.ImmediateOrder;
import orders.Order;
import market.Share;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;

import static org.junit.jupiter.api.Assertions.*;

public class ImmediateOrderTest {

    private Investor investor;
    private Share share;

    @BeforeEach
    public void setUp() {
        investor = new RandomInvestor(1, 0);
        investor.setCash(10000);
        investor.setShares(new int[]{100});
        share = new Share("AAPL", 150, 0);
    }

    @Test
    public void testImmediateOrderAttributes() {
        ImmediateOrder order = new ImmediateOrder(investor, 0, 0, 10, 150, 1);
        assertEquals(1, order.getTurn(), "Turn should be 1.");
        assertEquals(0, order.getTypeOfOrder(), "Order type should be 0 (immediate).");
    }

    @Test
    public void testCopyConstructor() {
        ImmediateOrder original = new ImmediateOrder(investor, 0, 0, 10, 150, 1);
        ImmediateOrder copy = new ImmediateOrder(original);

        assertEquals(original.getTurn(), copy.getTurn(), "Turn should be equal to the original order's turn.");
        assertEquals(original.getInvestor().getInvestorType(), copy.getInvestor().getInvestorType(), "Investor should be equal to the original order's investor.");
        assertEquals(original.getInvestor().getSharesLength(), copy.getInvestor().getSharesLength());
    }

    @Test
    public void testRealizeSale_RemovesOrderIfSharesExceed() {
        Order order = new ImmediateOrder(investor, 1, 0, 200, 150, 1);
        ArrayList<Order> saleOrders = new ArrayList<>();
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        saleOrders.add(order);

        order.realizeSale(1, saleOrders, purchaseOrders, 0, share);

        assertTrue(saleOrders.isEmpty(), "Order should be removed if shares exceed investor's shares.");
    }

    @Test
    public void testRealizeSale_RemovesOrderIfSharesZero() {
        Order order = new ImmediateOrder(investor, 1, 0, 0, 150, 1);
        ArrayList<Order> saleOrders = new ArrayList<>();
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        saleOrders.add(order);

        order.realizeSale(1, saleOrders, purchaseOrders, 0, share);

        assertTrue(saleOrders.isEmpty(), "Order should be removed if shares are zero.");
    }

    @Test
    public void testRealizePursue_RemovesOrderIfCashInsufficient() {
        investor.setCash(50);
        Order order = new ImmediateOrder(investor, 0, 0, 10, 150, 1);
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        ArrayList<Order> saleOrders = new ArrayList<>();
        purchaseOrders.add(order);

        order.realizePursue(1, purchaseOrders, saleOrders, 0, share);

        assertTrue(purchaseOrders.isEmpty(), "Order should be removed if cash is insufficient.");
    }

    @Test
    public void testRealizePursue_TransactionOccurs() {
        Order purchaseOrder = new ImmediateOrder(investor, 0, 0, 10, 150, 1);
        Investor seller = new RandomInvestor(2, 0);
        seller.setShares(new int[]{10});
        Order saleOrder = new ImmediateOrder(seller, 1, 0, 10, 150, 1);

        ArrayList<Order> purchaseOrders = new ArrayList<>();
        ArrayList<Order> saleOrders = new ArrayList<>();
        purchaseOrders.add(purchaseOrder);
        saleOrders.add(saleOrder);

        purchaseOrder.realizePursue(1, purchaseOrders, saleOrders, 0, share);

        assertEquals(8500, investor.getCash(), "Investor should have less cash after purchase.");
        assertEquals(110, investor.getNumberOfShares(0), "Investor should have more shares after purchase.");
        assertTrue(saleOrders.isEmpty(), "Sale order should be removed after transaction.");
    }
}