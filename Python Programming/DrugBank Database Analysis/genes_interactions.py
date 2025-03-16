"""
This script processes DrugBank XML data to extract gene-drug-product relationships and visualizes them as network graphs.

Modules:
    - pandas: Used for data manipulation and creating DataFrames.
    - networkx: Used for creating and visualizing network graphs.
    - matplotlib.pyplot: Used for plotting the network graph.
    - textwrap: Used for wrapping text in graph node labels.
    - data_read (custom): Contains the `load_drugbank_data` function for loading DrugBank XML data.

Functions:
    1. extract_gene_drug_data(drugs, namespace):
        Extracts data related to genes, drugs, and associated pharmaceutical products from the DrugBank XML dataset.

    2. draw_gene_drug_graph(gene_drug_df, gene_name):
        Visualizes the relationships between a specific gene, drugs targeting it, and their associated products as a network graph.

Execution:
    The script loads the DrugBank XML data, extracts gene-drug-product relationships, and visualizes a network graph for a specified gene.

Usage:
    Run the script to visualize gene-drug-product interactions for a specific gene by updating the `gene_name` variable.
"""

import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
import textwrap

from data_read import load_drugbank_data


""""
    Extracts gene-drug-product relationships from DrugBank XML data.

    Args:
        drugs (list): A list of XML elements representing drug entries from DrugBank.
        namespace (dict): A dictionary defining the XML namespaces for parsing.

    Returns:
        pd.DataFrame: A DataFrame containing columns:
                      - 'Gene': Name of the target gene.
                      - 'Drug': Name of the drug targeting the gene.
                      - 'Product': List of pharmaceutical product names associated with the drug.
"""
def extract_gene_drug_data(drugs, namespace):
    gene_drug_data = []

    for drug in drugs:
        drug_id = None
        drugbank_ids = drug.findall("ns:drugbank-id", namespace)
        for id_item in drugbank_ids:
            if id_item.attrib.get("primary") == "true":
                drug_id = id_item.text
                break

        if not drug_id:
            drug_id = drug.findtext("ns:drugbank-id", namespaces=namespace)

        drug_name = drug.findtext("ns:name", namespaces=namespace)
        products = drug.find("ns:products", namespaces=namespace)
        if products is not None:
            products = products.findall("ns:product", namespaces=namespace)
        else:
            products = []

        product_names = [product.findtext("ns:name", namespaces=namespace) for product in products]

        targets = drug.find("ns:targets", namespaces=namespace)
        if targets is not None:
            targets = targets.findall("ns:target", namespaces=namespace)
        else:
            targets = []

        for target in targets:
            gene_name = target.findtext("ns:name", namespaces=namespace)
            gene_drug_data.append({
                'Gene': gene_name,
                'Drug': drug_name,
                'Product': product_names
            })

    return pd.DataFrame(gene_drug_data)

"""
    Visualizes the gene-drug-product relationships for a specific gene as a network graph.

    Args:
        gene_drug_df (pd.DataFrame): A DataFrame containing columns:
                                     - 'Gene': Target gene name.
                                     - 'Drug': Drug name.
                                     - 'Product': List of pharmaceutical products.
        gene_name (str): The name of the gene to visualize.
"""
def draw_gene_drug_graph(gene_drug_df, gene_name):
    G = nx.Graph()

    # Filter the DataFrame for the specified gene
    filtered_df = gene_drug_df[gene_drug_df['Gene'] == gene_name]

    for _, row in filtered_df.iterrows():
        gene = row['Gene']
        drug = row['Drug']
        products = row['Product']

        G.add_node(gene, type='gene')
        G.add_node(drug, type='drug')
        G.add_edge(gene, drug)

        for product in products:
            G.add_node(product, type='product')
            G.add_edge(drug, product)

    pos = nx.spring_layout(G, k=0.5, seed=42)
    node_colors = ['lightblue' if G.nodes[node]['type'] == 'gene' else 'lightgreen' if G.nodes[node]['type'] == 'drug' else 'lightcoral' for node in G.nodes()]
    node_labels = {node: '\n'.join(textwrap.wrap(node, width=20)) for node in G.nodes()}

    plt.figure(figsize=(15, 10))
    nx.draw(G, pos, with_labels=True, labels=node_labels, node_color=node_colors, node_size=3000, font_size=10, font_weight='bold', edge_color='gray')
    plt.title(f'Gene-Drug-Product Interaction Network for {gene_name}')
    plt.show()

if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    gene_drug_df = extract_gene_drug_data(drugs, namespaces)
    gene_name = 'B-lymphocyte antigen CD20'  # Specify the gene name here
    draw_gene_drug_graph(gene_drug_df, gene_name)