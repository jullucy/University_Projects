package market;

import investors.Investor;
import investors.InvestorSma;
import investors.RandomInvestor;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.stream.IntStream;

/**
 * Represents the market system which manages investors, shares, and simulation turns.
 */
public class MarketSystem {
    private int turn; // Current turn of the simulation
    private final ArrayList<Investor> investors; // List of investors participating in the market
    private final ArrayList<Share> shares; // List of shares available for trading

    /**
     * Constructs a MarketSystem object by reading initial configuration from a file.
     * @param args Command-line arguments; expects file path as first argument.
     * @throws FileNotFoundException If the specified file path is not found.
     */
    public MarketSystem(String[] args) throws FileNotFoundException {
        if (args.length < 1) {
            throw new IllegalArgumentException("Please provide a file path as" +
                    " the first argument and number of turns as the second argument.");
        }

        investors = new ArrayList<>();
        shares = new ArrayList<>();
        turn = 0;

        String filePath = args[0];

        try (BufferedReader reader = new BufferedReader(new FileReader(filePath))) {
            String line;
            int counter = 3;

            // Process each line of the file based on the counter
            while ((line = reader.readLine()) != null && counter > 0) {
                if (!line.startsWith("#")) {
                    switch (counter) {
                        // Process line containing investor types.
                        case 3:
                            processInvestors(line);
                            break;
                        // Process line containing share identifiers and initial prices.
                        case 2:
                            processShares(line);
                            break;
                        // Process line containing initial cash and share holdings for investors.
                        case 1:
                            processCashAndShares(line);
                            break;
                    }
                    counter--;
                }
            }
        } catch (IOException e) {
            throw new IllegalArgumentException(e);
        }
    }

    /**
     * Processes the line containing investor types and initializes investors.
     * @param line Line from the input file containing investor types.
     */
    private void processInvestors(String line) {
        String[] words = line.trim().split("\\s+");
        int howMany = 0;

        for (String word : words) {
            switch (word) {
                // Create RandomInvestor.
                case "R":
                    investors.add(new RandomInvestor(howMany, 0));
                    howMany++;
                    break;
                // Create InvestorSma.
                case "S":
                    investors.add(new InvestorSma(howMany, 1));
                    howMany++;
                    break;
                default:
                    throw new IllegalArgumentException("Unknown type of investor: " + word);
            }
        }
    }

    /**
     * Processes the line containing share identifiers and initial prices,
     * and initializes shares.
     * @param line the line from the input file containing share identifiers and initial prices.
     */
    private void processShares(String line) {
        String[] words = line.trim().split("\\s+");
        int index = 0;

        for (String word : words) {
            String[] stocks = word.split(":");

            if (stocks.length == 2) {
                try {
                    String identifier = stocks[0];
                    int lastPrice = Integer.parseInt(stocks[1]);
                    shares.add(new Share(identifier, lastPrice, index)); // Create Share object
                    index++;
                } catch (NumberFormatException e) {
                    throw new NumberFormatException("Invalid integer: " + stocks[1]);
                }
            } else {
                throw new NumberFormatException("Invalid word format: " + word);
            }
        }
    }

    /**
     * Processes the line containing initial cash and share holdings for investors.
     * @param line Line from the input file containing initial cash and number of
     *             shares of every company for investors.
     */
    private void processCashAndShares(String line) {
        String[] words = line.trim().split("\\s+");
        int cash = Integer.parseInt(words[0]);
        int[] shareCounts = new int[shares.size()];

        for (int i = 1; i < words.length; i++) {
            String[] stocks = words[i].split(":");
            if (stocks.length == 2) {
                String identifier = stocks[0];
                int numberOfShares = Integer.parseInt(stocks[1]);

                for (int index = 0; index < shares.size(); index++) {
                    if (shares.get(index).getIdentifier().equals(identifier)) {
                        // Set initial number of shares.
                        shareCounts[index] = numberOfShares;
                        break;
                    }
                }
            } else {
                throw new NumberFormatException("Invalid word format: " + words[i]);
            }
        }

        // Set initial cash and number of shares for all investors
        IntStream.range(0, investors.size()).forEach(i -> {
            investors.get(i).setCash(cash);
            investors.get(i).setShares(shareCounts);
        });
    }

    public int getTurn() {
        return turn;
    }

    public int getSharesLength() {
        return shares.size();
    }

    public int getPrice(int index) {
        return shares.get(index).getLastPrice();
    }

    public Share getShares(int index) {
        return shares.get(index);
    }

    public ArrayList<Investor> getInvestors() {
        return investors;
    }

    public void increaseTurn() {
        turn++;
    }
}