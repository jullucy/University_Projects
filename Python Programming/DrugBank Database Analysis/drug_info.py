import pandas as pd
import matplotlib.pyplot as plt

from data_read import load_drugbank_data

"""
    Extracts and counts the statuses of drugs from the DrugBank XML data.

    Args:
        drugs (list): A list of XML elements representing drugs, parsed from the DrugBank XML file.
        namespaces (dict): A dictionary defining the XML namespaces used in the DrugBank file. 
                           Example: {'ns': 'http://www.drugbank.ca'}

    Returns:
        tuple: A tuple containing:
            - status_counts (dict): A dictionary with the following keys representing the counts of drug statuses:
                - 'approved': Count of drugs with the status "approved".
                - 'withdrawn': Count of drugs with the status "withdrawn".
                - 'experimental': Count of drugs with the status "experimental".
                - 'investigational': Count of drugs with the status "investigational".
                - 'vet_approved': Count of drugs with the status "vet_approved".
            - approved_not_withdrawn (int): The number of drugs that are "approved" but not "withdrawn".
"""
def extract_drug_status_data(drugs, namespaces):
    status_counts = {
        'approved': 0,
        'withdrawn': 0,
        'experimental': 0,
        'investigational': 0,
        'vet_approved': 0
    }
    approved_not_withdrawn = 0

    for drug in drugs:
        groups = drug.find("ns:groups", namespaces)  # Use namespace prefix
        if groups is not None:
            statuses = [group.text for group in groups.findall("ns:group", namespaces)]  # Use namespace prefix

            is_approved = 'approved' in statuses
            is_withdrawn = 'withdrawn' in statuses

            if is_approved:
                status_counts['approved'] += 1
                if not is_withdrawn:
                    approved_not_withdrawn += 1
            if is_withdrawn:
                status_counts['withdrawn'] += 1
            if 'experimental' in statuses:
                status_counts['experimental'] += 1
            if 'investigational' in statuses:
                status_counts['investigational'] += 1
            if 'vet_approved' in statuses:
                status_counts['vet_approved'] += 1

    return status_counts, approved_not_withdrawn

"""
    Plots a pie chart to visualize the distribution of drug statuses.

    Args:
        status_counts (dict): A dictionary containing the counts of drug statuses.
    The keys represent the status labels (e.g., 'approved', 'withdrawn'),
    and the values represent the corresponding counts.
"""
def plot_pie_chart(status_counts):
    labels = status_counts.keys()
    sizes = status_counts.values()

    plt.figure(figsize=(10, 7))
    plt.pie(sizes, labels=labels, autopct='%1.1f%%', startangle=140)
    plt.title('Drug Status Distribution')
    plt.show()

if __name__ == '__main__':
    file_path = "drugbank_partial.xml"

    # Define the namespace for XML parsing
    namespaces = {'ns': 'http://www.drugbank.ca'}

    # Load the DrugBank data
    drugs = load_drugbank_data(file_path, namespaces)

    # Extract and count statuses
    status_counts, approved_not_withdrawn = extract_drug_status_data(drugs, namespaces)

    # Create a DataFrame from the status counts
    status_df = pd.DataFrame(list(status_counts.items()), columns=['Status', 'Count'])
    print(status_df)

    # Plot the pie chart
    plot_pie_chart(status_counts)

    # Print the number of approved drugs that are not withdrawn
    print(f"Number of approved drugs that are not withdrawn: {approved_not_withdrawn}")
