import pandas as pd
import matplotlib.pyplot as plt

from data_read import load_drugbank_data
from pathways_plot import extract_pathway_interactions


"""
    Plots a histogram of the number of pathways associated with each DrugBank ID.
    If the number of pathways is not on the histogram, it means the drug has no pathways associated with it.

    Args:
        pathway_df (pd.DataFrame): A DataFrame containing pathway data, with a column named 'DrugBank ID'.
            - The 'DrugBank ID' column should contain the DrugBank IDs of drugs associated with pathways.

    Returns:
        None: The function displays a bar plot showing the number of pathways per DrugBank ID.
"""
def plot_histogram(pathway_df):
    drug_pathway_counts = pathway_df['DrugBank ID'].value_counts()

    plt.figure(figsize=(12, 8))
    drug_pathway_counts.plot(kind='bar', color='skyblue', edgecolor='black')
    plt.xlabel('DrugBank ID')
    plt.ylabel('Number of Pathways')
    plt.title('Number of Pathways per Drug')
    plt.xticks(rotation=90, fontsize=8)
    plt.tight_layout()
    plt.show()

if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    pathway_data = extract_pathway_interactions(drugs, namespaces)

    pathway_data.to_csv("pathway_interactions.csv", index=False)

    # Plot a histogram showing the number of pathways associated with each DrugBank ID.
    # If the drug is not on the histogram, it means it has no pathways associated with it.
    plot_histogram(pathway_data)