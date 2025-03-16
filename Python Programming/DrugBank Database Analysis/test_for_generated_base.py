import xml.etree.ElementTree as ET
import pandas as pd

from file_read import extract_primary_drug_data, load_drugbank_data
from project.pathways_plot import extract_pathway_interactions
from synonyms_searcher import extract_synonyms, draw_synonym_graph
from products import extract_product_data
from pathways import extract_pathway_data
from pathways_plot import create_bipartite_graph, plot_bipartite_graph, extract_pathway_interactions
from pathways_histogram import plot_histogram
from targets import extract_target_data
from targets_plot import plot_cellular_locations
from drug_info import extract_drug_status_data, plot_pie_chart
from interactions import extract_drug_interactions
from genes_interactions import extract_gene_drug_data, draw_gene_drug_graph
from my_own_analysis import analyse


if __name__ == '__main__':
    # File path
    file_path = "drugbank_partial_and_generated.xml"
    # Define the namespace for XML parsing
    namespace = {'ns': 'http://www.drugbank.ca'}

    drugs = load_drugbank_data(file_path, namespace)

    # Extract primary drug data and save to a CSV file.
    drug_df = extract_primary_drug_data(drugs, namespace)
    drug_df.to_csv("drug_data_summary_full.csv", index=False)

    # Extract synonyms into a DataFrame
    synonym_df = extract_synonyms(drugs, namespace)
    # Get user input and draw the synonym graph
    drug_id = input("Enter DrugBank ID: ")
    draw_synonym_graph(drug_id, synonym_df)

    # Extract product data and save to a CSV file.
    product_data = extract_product_data(drugs, namespace)
    product_data.to_csv("products_full.csv", index=False)

    # Extract pathway data and save to a CSV file.
    # Also, print the number of unique pathways.
    pathway_df, unique_pathway_count = extract_pathway_data(drugs, namespace)
    print(f"Number of unique pathways: {unique_pathway_count}")
    pathway_df.to_csv("pathway_full.csv", index=False)

    # Extract pathway data and save to a CSV file.
    pathway_data_interactions = extract_pathway_interactions(drugs, namespace)
    pathway_data_interactions.to_csv("pathway_interactions_full.csv", index=False)
    # Create and plot the bipartite graph
    B = create_bipartite_graph(pathway_data_interactions)
    plot_bipartite_graph(B)

    # Plot a histogram showing the number of pathways associated with each DrugBank ID.
    # If the drug is not on the histogram, it means it has no pathways associated with it.
    plot_histogram(pathway_data_interactions)

    # Extract target data and save to a CSV file.
    target_data = extract_target_data(drugs, namespace)
    target_data.to_csv("targets_full.csv", index=False)

    # Plot the distribution of cellular locations for drug targets.
    plot_cellular_locations("targets_full.csv")

    # Extract and count statuses
    status_counts, approved_not_withdrawn = extract_drug_status_data(drugs, namespace)
    # Create a DataFrame from the status counts
    status_df = pd.DataFrame(list(status_counts.items()), columns=['Status', 'Count'])
    print(status_df)
    # Plot the pie chart
    plot_pie_chart(status_counts)
    # Print the number of approved drugs that are not withdrawn
    print(f"Number of approved drugs that are not withdrawn: {approved_not_withdrawn}")

    # Extract drug interactions, save the interactions to a CSV file
    interaction_df = extract_drug_interactions(drugs, namespace)
    interaction_df.to_csv("drug_interactions_full.csv", index=False)

    # Extract gene-drug data and draw the network graph for a specific gene
    gene_drug_df = extract_gene_drug_data(drugs, namespace)
    gene_name = 'B-lymphocyte antigen CD20'  # Specify the gene name here
    draw_gene_drug_graph(gene_drug_df, gene_name)

    # Extract informations from UniProt and display the results
    # It takes a DrugBank ID as input and displays the extracted data.
    # Also, save the results to a CSV file and plot the taxonomy star network
    analyse(drugs, namespace)