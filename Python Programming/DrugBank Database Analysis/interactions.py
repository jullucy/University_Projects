import pandas as pd
from data_read import load_drugbank_data


"""
    Extract potential drug interactions from the DrugBank database.

    Parameters:
    - root: The root element of the parsed XML tree.
    - namespaces: Dictionary containing namespace information for parsing.

    Returns:
    - A pandas DataFrame with the following columns:
      - DrugBank ID (Primary)
      - Interacting DrugBank ID
      - Interaction Description
"""
def extract_drug_interactions(drugs, namespaces):
    interaction_data = []

    for drug in drugs:
        # Find the primary DrugBank ID
        primary_id = None
        drugbank_ids = drug.findall("ns:drugbank-id", namespaces)
        for db_id in drugbank_ids:
            if db_id.attrib.get("primary") == "true":
                primary_id = db_id.text
                break

        if not primary_id:
            continue

        # Find drug interactions
        interactions = drug.find("ns:drug-interactions", namespaces)
        if interactions is not None:
            for interaction in interactions.findall("ns:drug-interaction", namespaces):
                interacting_drug_id = interaction.findtext("ns:drugbank-id", default="No data", namespaces=namespaces)
                interaction_description = interaction.findtext("ns:description", default="No description", namespaces=namespaces)

                # Append interaction data to the list
                interaction_data.append({
                    "DrugBank ID (Primary)": primary_id,
                    "Interacting DrugBank ID": interacting_drug_id,
                    "Interaction Description": interaction_description
                })

    # Convert the list of interactions to a DataFrame
    return pd.DataFrame(interaction_data)


if __name__ == "__main__":
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    # Extract drug interactions
    interaction_df = extract_drug_interactions(drugs, namespaces)

    # Save the interactions to a CSV file
    interaction_df.to_csv("drug_interactions.csv", index=False)
