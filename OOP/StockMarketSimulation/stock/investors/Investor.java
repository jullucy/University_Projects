package investors;

import market.MarketSystem;
import orders.*;

import static investors.Draw.draw;

public abstract class Investor {
    private int cash;
    private int[] shares; // Sorted array of number of shares - every value in
    // array is holding the information about how many shares of this company
    // the investor has.
    private final int number; // Investor identifier.
    private final int investorType; // 0 means random investor, 1 means SMA.

    /**
     * @param number Number of investor (it's his identifier).
     * @param type is the type of investor, 0 means random investor,
     * 1 means SMA investor.
     */
    public Investor(int number, int type) {
        this.number = number;
        this.investorType = type;
    }

    /**
     * Class method used by subclasses to copy given investor.
     */
    protected Investor(Investor investor) {
        this.number = investor.getNumber();
        this.cash = investor.getCash();
        this.shares = new int[investor.getSharesLength()];
        this.investorType = investor.getInvestorType();

        for (int i = 0; i < investor.getSharesLength(); i++) {
            this.shares[i] = investor.getNumberOfShares(i);
        }
    }

    public int getInvestorType() {
        return investorType;
    }

    public int getNumber() {
        return number;
    }

    public int getSharesLength() {
        return shares.length;
    }

    public int getCash() {
        return cash;
    }

    public int getNumberOfShares(int index) {
        return shares[index];
    }

    public void setCash(int cash) {
        this.cash = cash;
    }

    public void setShares(int[] shares) {
        this.shares = new int[shares.length];
        System.arraycopy(shares, 0, this.shares, 0, shares.length);
    }

    /**
     * Method used while sorting the investors array, compare investor with given
     * investor by their numbers.
     * @param other the investor with who it is being compared.
     * @return 0 if investor's numbers are equal, 1 if given investor has lower
     * number and -1 if he has higher number.
     */
    public int compareNumber(Investor other) {
        return Integer.compare(this.number, other.getNumber());
    }

    public void sellShares(int shareIndex, int price, int numberOfShares) {
        this.shares[shareIndex] -= numberOfShares;
        this.cash += price * numberOfShares;
    }

    public void buyShares(int shareIndex, int price, int numberOfShares) {
        this.shares[shareIndex] += numberOfShares;
        this.cash -= price * numberOfShares;
    }

    /**
     * Method that controls the way the investor is making decisions.
     * It has to be implemented in every new investor subclass.
     * @param system market which the investor is using.
     */
    public abstract void makeDecision(MarketSystem system);

    /**
     * Helper method for decision-making:
     * draws the type of order,
     * draws the price the investor want to give,
     * creates the order and give it to the system.
     * @param system market which the investor is using.
     * @param type type of order - is it a sale or a purchase order. 0 stands
     *             for purchase, 1 stands for sale.
     * @param indexInShares index in shares array.
     */
    void decide(MarketSystem system, int type, int indexInShares) {
        Order pursueOrder;
        int typeOfOrder = draw(0, 3);
        int price = draw(-10, 10) + system.getPrice(indexInShares);

        // Price cannot be a negative number.
        if (price <= 0) {
            price += 10;
        }
        // We don't want for the investor to make many orders without the deadline,
        // because they lower the efficiency of the simulation.
        if (typeOfOrder == 1 && draw(0, 100) != 0) {
            typeOfOrder = 0;
        }

        int howManyShares = (type == 0) ? getCash() / price : getNumberOfShares(indexInShares);

        switch (typeOfOrder) {
            case 0 -> pursueOrder = new ImmediateOrder(this, type,
                    system.getShares(indexInShares).getIndex(), howManyShares, price, system.getTurn());
            case 1 -> pursueOrder = new OrderWithoutDeadline(this, type,
                    system.getShares(indexInShares).getIndex(), howManyShares, price);
            case 2 -> pursueOrder = new CompleteOrCancelOrder(this, type,
                    system.getShares(indexInShares).getIndex(), howManyShares, price, system.getTurn());
            case 3 -> {
                int turn = draw(system.getTurn(), system.getTurn() + 1000);
                pursueOrder = new OrderValidUntilGivenTurn(this, type,
                        system.getShares(indexInShares).getIndex(), howManyShares, price, turn);
            }

            default -> throw new IllegalStateException("Unexpected value: " + typeOfOrder);
        }

        system.getShares(indexInShares).addOrder(pursueOrder);
    }
}