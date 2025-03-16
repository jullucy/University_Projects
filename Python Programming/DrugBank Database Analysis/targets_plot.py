import pandas as pd
import matplotlib.pyplot as plt

from data_read import load_drugbank_data
from targets import extract_target_data


"""
    Visualizes the distribution of cellular locations for drug targets as a pie chart.

    Args:
        file_path (str): Path to the CSV file containing drug target data. 
                         The file must have a column named 'Cellular Location' with location names.

    Returns:
        None: The function displays a pie chart showing the percentage distribution of cellular locations.
"""
def plot_cellular_locations(file_path):
    # Read the CSV file
    df = pd.read_csv(file_path)

    # Group by 'Cellular Location' and count the number of occurrences
    cellular_location_counts = df['Cellular Location'].value_counts()

    # Calculate the percentage of each location
    total = cellular_location_counts.sum()
    percentages = (cellular_location_counts / total) * 100

    # Filter out locations with less than 3%
    filtered_counts = cellular_location_counts[percentages >= 3]
    other_counts = cellular_location_counts[percentages < 3]
    other_total = other_counts.sum()

    # Add 'Other' category if there are locations with less than 3%
    if other_total > 0:
        filtered_counts['Other'] = other_total

    # Create a pie chart
    plt.figure(figsize=(10, 15))
    wedges, texts, autotexts = plt.pie(filtered_counts, labels=filtered_counts.index, autopct=lambda p: f'{p:.1f}%' if p >= 3 else '', startangle=140, textprops={'fontsize': 14})
    plt.title('Distribution of Cellular Locations for Drug Targets', fontsize=24)
    plt.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle

    # Create legend labels
    legend_labels = [f'{loc} ({count / total:.1%})' for loc, count in filtered_counts.items()]
    if other_total > 0:
        other_labels = [f'{loc} ({count / total:.1%})' for loc, count in other_counts.items()]
        legend_labels[-1] += ':\n' + '\n'.join(other_labels)

    # Add a legend at the bottom
    plt.legend(wedges, legend_labels, title="Cellular Locations", loc="upper center", bbox_to_anchor=(0.5, -0.05), ncol=2, fontsize=12, title_fontsize=14)
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    target_data = extract_target_data(drugs, namespaces)
    target_data.to_csv("targets.csv", index=False)
    plot_cellular_locations("targets.csv")