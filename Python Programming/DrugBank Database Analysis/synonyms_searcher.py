import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import textwrap
from data_read import load_drugbank_data

"""
    Extracts synonyms for each drug from a DrugBank XML file.

    Args:
        drugs (list of xml.etree.ElementTree.Element): List of <drug> elements parsed from the DrugBank XML file.
        namespace (dict): Namespace dictionary for XML parsing, e.g., {'ns': 'http://www.drugbank.ca'}.

    Returns:
        pd.DataFrame: A DataFrame containing the following columns:
            - "Primary DrugBank ID": The primary DrugBank ID for the drug.
            - "Synonyms": A list of synonyms associated with the drug.
    """
def extract_synonyms(drugs, namespace):
    synonym_data = []

    for drug in drugs:
        # Get the primary DrugBank ID
        primary_id = None
        drugbank_ids = drug.findall("ns:drugbank-id", namespace)
        for db_id in drugbank_ids:
            if db_id.attrib.get("primary") == "true":
                primary_id = db_id.text
                break

        if not primary_id:
            continue

        # Extract synonyms
        synonyms = []
        synonyms_element = drug.find("ns:synonyms", namespace)
        if synonyms_element is not None:
            for synonym in synonyms_element.findall("ns:synonym", namespace):
                if synonym.text:
                    synonyms.append(synonym.text.strip())

        synonym_data.append({
            "Primary DrugBank ID": primary_id,
            "Synonyms": synonyms
        })

    # Convert to DataFrame
    return pd.DataFrame(synonym_data)

"""
    Draws a graph of synonyms for a given DrugBank ID.
    Handles long synonym names by wrapping text using newlines.
    """
def draw_synonym_graph(drugbank_id, synonym_df):
    # Find synonyms for the given DrugBank ID
    row = synonym_df[synonym_df['Primary DrugBank ID'] == drugbank_id]

    if row.empty:
        print(f"No synonyms found for DrugBank ID: {drugbank_id}")
        return

    synonyms = row.iloc[0]['Synonyms']

    # Create a graph
    G = nx.Graph()
    G.add_node(drugbank_id)  # Add the main DrugBank ID as the central node

    # Add synonym nodes and edges
    for synonym in synonyms:
        G.add_node(synonym)
        G.add_edge(drugbank_id, synonym)

    # Adjust layout
    pos = nx.spring_layout(G, k=0.5, seed=42)  # Adjust k for node spacing

    # Wrap long labels
    wrapped_labels = {node: '\n'.join(textwrap.wrap(node, width=20)) for node in G.nodes()}

    # Draw the graph
    plt.figure(figsize=(12, 8))  # Larger figure for better readability
    nx.draw_networkx_nodes(G, pos, node_size=8000, node_color="lightblue")
    nx.draw_networkx_edges(G, pos, width=1.5, alpha=0.7, edge_color="gray")
    nx.draw_networkx_labels(G, pos, labels=wrapped_labels, font_size=10, font_color="black", font_weight="bold")

    plt.title(f"Synonym Graph for DrugBank ID: {drugbank_id}")
    plt.axis("off")
    plt.tight_layout()
    plt.show()


# Main function to load DrugBank data and draw synonym graph
if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    # Extract synonyms into a DataFrame
    synonym_df = extract_synonyms(drugs, namespaces)

    # Get user input and draw the synonym graph
    drug_id = input("Enter DrugBank ID: ")
    draw_synonym_graph(drug_id, synonym_df)
