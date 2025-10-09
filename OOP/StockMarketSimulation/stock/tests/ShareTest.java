package tests;
import market.Share;
import orders.Order;
import orders.OrderValidUntilGivenTurn;
import investors.Investor;
import investors.RandomInvestor;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class ShareTest {

    @Test
    public void testAddOrder() {
        Investor investor = new RandomInvestor(0, 0);
        Order order = new OrderValidUntilGivenTurn(investor, 0, 1, 10, 100, 10);
        Share share = new Share("ABC", 100, 0);
        share.addOrder(order);

        assertEquals(1, share.getOrders().getPurchaseOrders().size(), "Order should be added to purchaseOrders");
    }

    @Test
    public void testSetLastPrice() {
        Share share = new Share("ABC", 100, 0);
        share.setLastPrice(200);

        assertEquals(200, share.getLastPrice(), "Last price should be updated");
    }

    @Test
    public void testComplete() {
        Investor seller = new RandomInvestor(0, 0);
        Order saleOrder = new OrderValidUntilGivenTurn(seller, 1, 0, 10, 100, 10);

        Investor buyer = new RandomInvestor(1, 0);
        Order purchaseOrder = new OrderValidUntilGivenTurn(buyer, 0, 0, 10, 100, 10);
        buyer.setShares(new int[]{10});
        buyer.setCash(10000);

        Share share = new Share("XYZ", 100, 0);
        seller.setShares(new int[]{10});
        seller.setCash(100000);

        share.addOrder(saleOrder);
        share.addOrder(purchaseOrder);
        share.complete(5);

        assertEquals(0, share.getOrders().getPurchaseOrders().size(), "Purchase order should be completed");
        assertEquals(0, share.getOrders().getSaleOrders().size(), "Sale order should be completed");
    }
}