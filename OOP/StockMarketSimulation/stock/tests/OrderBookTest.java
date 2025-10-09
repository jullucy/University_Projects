package tests;

import market.OrderBook;
import market.Share;
import orders.Order;
import orders.OrderValidUntilGivenTurn;
import investors.Investor;
import investors.RandomInvestor;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class OrderBookTest {

    @Test
    public void testAddOrder() {
        Investor investor = new RandomInvestor(0, 0);
        Order order = new OrderValidUntilGivenTurn(investor, 0, 1, 10, 100, 10);
        OrderBook orderBook = new OrderBook();
        orderBook.addOrder(order);

        assertEquals(1, orderBook.getPurchaseOrders().size(), "Order should be added to purchaseOrders");
    }

    @Test
    public void testComplete() {
        Investor seller = new RandomInvestor(0, 0);
        Order saleOrder = new OrderValidUntilGivenTurn(seller, 1, 0, 10, 100, 10);
        seller.setShares(new int[]{10});
        seller.setCash(100000);
        Investor buyer = new RandomInvestor(1, 0);
        buyer.setShares(new int[]{10});
        buyer.setCash(100000);
        Order purchaseOrder = new OrderValidUntilGivenTurn(buyer, 0, 0, 10, 100, 10);

        Share share = new Share("XYZ", 100, 0);

        OrderBook orderBook = new OrderBook();
        orderBook.addOrder(saleOrder);
        orderBook.addOrder(purchaseOrder);
        orderBook.complete(5, share);

        assertEquals(0, orderBook.getPurchaseOrders().size(), "Purchase order should be completed");
        assertEquals(0, orderBook.getSaleOrders().size(), "Sale order should be completed");
    }
}
