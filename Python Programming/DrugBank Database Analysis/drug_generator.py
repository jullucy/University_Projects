import xmltodict
import random
import copy

# Load existing drug data
def load_drugbank_data(file_path):
    """Load the DrugBank XML file into a dictionary."""
    with open(file_path, 'r', encoding='utf-8') as file:
        data = xmltodict.parse(file.read())
    return data

"""
    Generate new drugs by randomly sampling the existing drugs,
    modifying the 'drugbank-id' and optionally randomizing other fields.
"""
def generate_additional_drugs(existing_drugs, num_new_drugs=19900):
    new_drugs = []
    base_id = 100000  # Starting ID for new drugs

    for i in range(num_new_drugs):
        # Deep copy an existing drug
        new_drug = copy.deepcopy(random.choice(existing_drugs))

        # Modify the primary 'drugbank-id'
        if isinstance(new_drug['drugbank-id'], list):
            for drugbank_id in new_drug['drugbank-id']:
                if isinstance(drugbank_id, dict) and drugbank_id.get('@primary') == 'true':
                    drugbank_id['#text'] = f'DB{base_id + i:05d}'
                    break
        elif isinstance(new_drug['drugbank-id'], dict):
            if new_drug['drugbank-id'].get('@primary') == 'true':
                new_drug['drugbank-id']['#text'] = f'DB{base_id + i:05d}'

        # Add the modified drug to the new list
        new_drugs.append(new_drug)

    return new_drugs

"""
    Save the combined DrugBank data into a new XML file.
"""
def save_combined_data(file_path, combined_data):
    with open(file_path, 'w', encoding='utf-8') as file:
        xmltodict.unparse(combined_data, file, pretty=True)

def main():
    # File paths
    input_file_path = 'drugbank_partial.xml'
    output_file_path = 'drugbank_partial_and_generated.xml'

    # Load the base DrugBank data
    data = load_drugbank_data(input_file_path)

    # Ensure the 'drug' field exists and is a list
    if not isinstance(data['drugbank']['drug'], list):
        data['drugbank']['drug'] = [data['drugbank']['drug']]

    # Extract only the <drug> elements (exclude other sections)
    existing_drugs = data['drugbank']['drug']

    # Generate additional drugs
    additional_drugs = generate_additional_drugs(existing_drugs)

    # Combine existing and new drugs
    combined_drugs = existing_drugs + additional_drugs
    data['drugbank']['drug'] = combined_drugs

    # Save the new database
    save_combined_data(output_file_path, data)
    print(f"Generated file saved to: {output_file_path}")

if __name__ == '__main__':
    main()
