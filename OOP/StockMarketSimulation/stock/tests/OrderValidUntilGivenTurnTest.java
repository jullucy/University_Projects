package tests;

import investors.Investor;
import investors.RandomInvestor;
import market.Share;
import orders.Order;
import orders.OrderValidUntilGivenTurn;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;

import static org.junit.jupiter.api.Assertions.*;

public class OrderValidUntilGivenTurnTest {
    @Test
    public void testRealizeSale() {
        Investor investor = new RandomInvestor(0, 0);
        investor.setCash(1000);
        investor.setShares(new int[]{10});
        Investor investor2 = new RandomInvestor(0, 0);
        investor2.setCash(1000);
        investor2.setShares(new int[]{10});

        Share share = new Share("ABC", 100, 0);

        OrderValidUntilGivenTurn saleOrder = new OrderValidUntilGivenTurn(investor, 1, 0, 5, 90, 10);
        ArrayList<Order> saleOrders = new ArrayList<>();
        saleOrders.add(saleOrder);
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        OrderValidUntilGivenTurn purchaseOrder = new OrderValidUntilGivenTurn(investor2, 0, 0, 5, 90, 10);
        purchaseOrders.add(purchaseOrder);

        saleOrder.realizeSale(5, saleOrders, purchaseOrders, 0, share);

        assertEquals(0, saleOrders.size(), "Sale order should be removed after realization");
    }

    @Test
    public void testRealizePursue() {
        Investor investor = new RandomInvestor(0, 0);
        investor.setCash(1000);
        investor.setShares(new int[]{10});
        Investor investor2 = new RandomInvestor(0, 0);
        investor2.setCash(1000);
        investor2.setShares(new int[]{10});

        Share share = new Share("ABC", 100, 0);

        OrderValidUntilGivenTurn purchaseOrder = new OrderValidUntilGivenTurn(investor, 0, 0, 5, 90, 10);
        ArrayList<Order> purchaseOrders = new ArrayList<>();
        purchaseOrders.add(purchaseOrder);

        OrderValidUntilGivenTurn saleOrder = new OrderValidUntilGivenTurn(investor2, 1, 0, 5, 90, 10);
        ArrayList<Order> saleOrders = new ArrayList<>();
        saleOrders.add(saleOrder);

        purchaseOrder.realizePursue(5, purchaseOrders, saleOrders, 0, share);

        assertEquals(0, purchaseOrders.size(), "Purchase order should be removed after realization");
    }
}
