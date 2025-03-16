import pandas as pd

from data_read import load_drugbank_data


"""
    Extracts primary drug information from a list of drugs parsed from a DrugBank XML file.

    Args:
        drugs (list of xml.etree.ElementTree.Element): List of <drug> elements from the DrugBank XML file.
        namespaces (dict): Namespace dictionary for XML parsing, e.g., {'ns': 'http://www.drugbank.ca'}.

    Returns:
        pd.DataFrame: A DataFrame containing processed drug information, including:
            - Primary DrugBank ID
            - Name
            - Type
            - Description
            - Form
            - Indications
            - Mechanism of Action
            - Food Interactions
    """
def extract_primary_drug_data(drugs, namespaces):
    drug_info = []

    for drug in drugs:
            primary_id = None
            drugbank_ids = drug.findall("ns:drugbank-id", namespaces)

            if drugbank_ids:
                for id_item in drugbank_ids:
                    if id_item.get("primary") == "true":  # Check for primary="true"
                        primary_id = id_item.text
                        break

            if not primary_id:
                continue

            # Extract other drug attributes
            name = drug.findtext("ns:name", namespaces=namespaces)
            drug_type = drug.get("type")
            description = drug.findtext("ns:description", namespaces=namespaces)

            # Extract unique dosage forms
            dosages = drug.find("ns:dosages", namespaces=namespaces)

            if dosages is not None:
                forms = set()
                for dosage in dosages.findall("ns:dosage", namespaces=namespaces):
                    form = dosage.findtext("ns:form", namespaces=namespaces)

                    if form:
                        forms.add(form)

                form = ", ".join(forms) if forms else "No data found."
            else:
                form = "No data found."

            indications = drug.findtext("ns:indication", namespaces=namespaces)
            mechanism_of_action = drug.findtext("ns:mechanism-of-action", namespaces=namespaces)

            # Extract food interactions
            food_interactions = drug.find("ns:food-interactions", namespaces=namespaces)

            if food_interactions is not None:
                interactions = [fi.text for fi in food_interactions.findall("ns:food-interaction", namespaces=namespaces)]
                food_interactions_text = ", ".join(interactions) if interactions else "No interactions found."
            else:
                food_interactions_text = "No interactions found."

            # Append the processed drug data
            drug_info.append({
                'Primary DrugBank ID': primary_id,
                'Name': name,
                'Type': drug_type,
                'Description': description,
                'Form': form,
                'Indications': indications,
                'Mechanism of Action': mechanism_of_action,
                'Food Interactions': food_interactions_text
            })


    return pd.DataFrame(drug_info)

# Main for usage of this function only
if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    drug_df = extract_primary_drug_data(drugs, namespaces)

    # Save the DataFrame to a CSV file
    drug_df.to_csv("drug_data_summary.csv", index=False)