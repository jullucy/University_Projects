"""
This module contains a series of unit tests for various functions in the
DrugBank data processing pipeline. These tests cover different aspects such
as loading XML data, extracting drug information, counting drug interactions,
and validating expected outcomes.

Modules being tested:
- `data_read.py`: Loading DrugBank data.
- `file_read.py`: Extracting primary drug data.
- `drug_info.py`: Extracting drug status data.
- `interactions.py`: Extracting drug interaction data.

Each test is designed to simulate real-world cases, ensuring that the
functions handle expected and unexpected inputs properly, and return the
correct results.
"""

import pytest
import pandas as pd

from unittest.mock import MagicMock, patch
from xml.etree.ElementTree import Element, tostring, SubElement
from data_read import load_drugbank_data
from file_read import extract_primary_drug_data
from drug_info import extract_drug_status_data
from interactions import extract_drug_interactions


# Tests for the data_read.py module

"""
    Fixture providing a small sample of XML data.

    Returns:
        str: XML content with mock drug information.
"""
@pytest.fixture
def sample_xml():
    xml_content = """
    <drugbank xmlns="http://www.drugbank.ca">
        <drug>
            <drugbank-id primary="true">DB001</drugbank-id>
            <name>DrugA</name>
        </drug>
        <drug>
            <drugbank-id primary="true">DB002</drugbank-id>
            <name>DrugB</name>
        </drug>
    </drugbank>
    """
    return xml_content

"""
    Test loading the DrugBank data from an XML file.

    Args:
        mock_parse (MagicMock): Mocked version of XML parsing function.
        sample_xml (str): Sample XML content for testing.
"""
@patch("xml.etree.ElementTree.parse")
def test_load_drugbank_data(mock_parse, sample_xml):
    # Mocking the results of XML parsing
    mock_tree = MagicMock()
    mock_tree.getroot.return_value = Element("drugbank", xmlns="http://www.drugbank.ca")
    mock_parse.return_value = mock_tree

    namespaces = {"ns": "http://www.drugbank.ca"}
    drugs = load_drugbank_data("dummy_path.xml", namespaces)

    assert drugs is not None
    assert isinstance(drugs, list)


# Tests for count_drug_information() in server module.

"""
    Test counting pathways for a given drug with parameterization.

    Args:
        mock_count (MagicMock): Mocked function for counting drug interactions.
        mock_load (MagicMock): Mocked function for loading DrugBank data.
        drug_id (str): The drug ID being tested.
        expected_count (int): Expected count of drug interactions.
"""
@pytest.mark.parametrize(
    "drug_id,expected_count",
    [
        ("DB001", 3),
        ("DB002", 0),
        ("INVALID", 0),
    ]
)
@patch("server.load_drugbank_data")
@patch("server.count_drug_interactions")
def test_count_drug_interactions(mock_count, mock_load, drug_id, expected_count):
    mock_root = MagicMock()
    mock_load.return_value = mock_root
    mock_count.return_value = expected_count

    # Wywołanie funkcji i weryfikacja wyniku
    from server import count_drug_interactions
    result = count_drug_interactions(mock_root, drug_id)

    assert result == expected_count

"""
    Test behavior when a drug ID is not found.

    Asserts that a 404 error is raised when a drug ID is missing.
"""
def test_missing_drug_id():
    from fastapi import HTTPException
    from server import count_drug_interactions

    mock_root = MagicMock()

    with pytest.raises(HTTPException) as excinfo:
        count_drug_interactions(mock_root, "INVALID_ID")

    assert excinfo.value.status_code == 404
    assert "not found" in excinfo.value.detail


# Tests for the file_read.py module

"""
    Fixture that creates a mock list of <drug> elements for testing.

    Returns:
        tuple: A list of drug elements and a mock namespace.
"""
@pytest.fixture
def mock_drugs():
    # Create a mock namespace
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock XML drug elements
    drug1 = Element("{http://www.drugbank.ca}drug", attrib={"type": "small molecule"})
    SubElement(drug1, "{http://www.drugbank.ca}drugbank-id", attrib={"primary": "true"}).text = "DB001"
    SubElement(drug1, "{http://www.drugbank.ca}name").text = "Aspirin"
    SubElement(drug1, "{http://www.drugbank.ca}description").text = "Pain reliever"
    SubElement(drug1, "{http://www.drugbank.ca}mechanism-of-action").text = "Inhibits COX enzymes"
    SubElement(drug1, "{http://www.drugbank.ca}indication").text = "Used for pain relief"
    dosages = SubElement(drug1, "{http://www.drugbank.ca}dosages")
    dosage1 = SubElement(dosages, "{http://www.drugbank.ca}dosage")
    SubElement(dosage1, "{http://www.drugbank.ca}form").text = "Tablet"

    drug2 = Element("{http://www.drugbank.ca}drug", attrib={"type": "biotech"})
    SubElement(drug2, "{http://www.drugbank.ca}drugbank-id", attrib={"primary": "true"}).text = "DB002"
    SubElement(drug2, "{http://www.drugbank.ca}name").text = "Insulin"
    SubElement(drug2, "{http://www.drugbank.ca}description").text = "Hormone therapy"
    SubElement(drug2, "{http://www.drugbank.ca}mechanism-of-action").text = "Regulates glucose levels"
    SubElement(drug2, "{http://www.drugbank.ca}indication").text = "Used for diabetes"

    return [drug1, drug2], ns

"""
    Test the `extract_primary_drug_data` function.

    Args:
        mock_drugs (tuple): Mocked list of drug elements and namespace.
"""
def test_extract_primary_drug_data(mock_drugs):
    drugs, namespaces = mock_drugs

    # Call the function
    df = extract_primary_drug_data(drugs, namespaces)

    # Assertions
    assert isinstance(df, pd.DataFrame)
    assert len(df) == 2
    assert list(df.columns) == [
        "Primary DrugBank ID",
        "Name",
        "Type",
        "Description",
        "Form",
        "Indications",
        "Mechanism of Action",
        "Food Interactions",
    ]
    assert df.loc[0, "Primary DrugBank ID"] == "DB001"
    assert df.loc[0, "Name"] == "Aspirin"
    assert df.loc[0, "Form"] == "Tablet"
    assert df.loc[1, "Primary DrugBank ID"] == "DB002"
    assert df.loc[1, "Name"] == "Insulin"


# Tests for the drug_info.py module

"""
    Fixture that creates a mock list of <drug> elements for testing drug status.

    Returns:
        tuple: A list of drug elements and a mock namespace.
"""
@pytest.fixture
def mock_drugs_info():
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock XML drug elements
    drug1 = Element("{http://www.drugbank.ca}drug")
    groups1 = SubElement(drug1, "{http://www.drugbank.ca}groups")
    SubElement(groups1, "{http://www.drugbank.ca}group").text = "approved"
    SubElement(groups1, "{http://www.drugbank.ca}group").text = "withdrawn"

    drug2 = Element("{http://www.drugbank.ca}drug")
    groups2 = SubElement(drug2, "{http://www.drugbank.ca}groups")
    SubElement(groups2, "{http://www.drugbank.ca}group").text = "approved"
    SubElement(groups2, "{http://www.drugbank.ca}group").text = "experimental"

    drug3 = Element("{http://www.drugbank.ca}drug")
    groups3 = SubElement(drug3, "{http://www.drugbank.ca}groups")
    SubElement(groups3, "{http://www.drugbank.ca}group").text = "vet_approved"

    drug4 = Element("{http://www.drugbank.ca}drug")
    # No groups element for drug4

    return [drug1, drug2, drug3, drug4], ns

"""
    Test the `extract_drug_status_data` function with valid data.

    Args:
        mock_drugs_info (tuple): Mocked list of drug elements and namespace.
    """
def test_extract_drug_status_data(mock_drugs_info):
    drugs, namespaces = mock_drugs_info

    # Call the function
    status_counts, approved_not_withdrawn = extract_drug_status_data(drugs, namespaces)

    # Expected status counts
    expected_status_counts = {
        'approved': 2,
        'withdrawn': 1,
        'experimental': 1,
        'investigational': 0,
        'vet_approved': 1
    }
    expected_approved_not_withdrawn = 1

    # Assertions
    assert status_counts == expected_status_counts
    assert approved_not_withdrawn == expected_approved_not_withdrawn

"""
    Parameterized test for `extract_drug_status_data`.

    Args:
        mock_groups (list): List of drug groups to be tested.
        expected_counts (dict): Expected status counts.
        expected_approved_not_withdrawn (int): Expected approved but not withdrawn count.
"""
@pytest.mark.parametrize(
    "mock_groups,expected_counts,expected_approved_not_withdrawn",
    [
        # Test case 1: Only 'approved'
        ([("approved",)], {'approved': 1, 'withdrawn': 0, 'experimental': 0, 'investigational': 0, 'vet_approved': 0}, 1),
        # Test case 2: 'approved' and 'withdrawn'
        ([("approved", "withdrawn")], {'approved': 1, 'withdrawn': 1, 'experimental': 0, 'investigational': 0, 'vet_approved': 0}, 0),
        # Test case 3: No groups
        ([], {'approved': 0, 'withdrawn': 0, 'experimental': 0, 'investigational': 0, 'vet_approved': 0}, 0),
    ],
)
def test_extract_drug_status_data_parametrized(mock_groups, expected_counts, expected_approved_not_withdrawn):
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock drugs
    drugs = []
    for groups in mock_groups:
        drug = Element("{http://www.drugbank.ca}drug")
        groups_elem = SubElement(drug, "{http://www.drugbank.ca}groups")
        for group in groups:
            SubElement(groups_elem, "{http://www.drugbank.ca}group").text = group
        drugs.append(drug)

    # Call the function
    status_counts, approved_not_withdrawn = extract_drug_status_data(drugs, ns)

    # Assertions
    assert status_counts == expected_counts
    assert approved_not_withdrawn == expected_approved_not_withdrawn

"""
    Test behavior when no drug status groups are found.

    Asserts that the status counts are all zeros.
"""
def test_extract_drug_status_data_no_groups():
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock drugs with no groups element
    drugs = [Element("drug") for _ in range(3)]

    # Call the function
    status_counts, approved_not_withdrawn = extract_drug_status_data(drugs, ns)

    # Assertions
    assert status_counts == {
        'approved': 0,
        'withdrawn': 0,
        'experimental': 0,
        'investigational': 0,
        'vet_approved': 0
    }
    assert approved_not_withdrawn == 0


# Tests for interactions.py module

"""
    Fixture that creates mock drug interaction data for testing.

    Returns:
        list: A list of mock drug interactions.
"""
@pytest.fixture
def mock_drugs_interactions():
    """Fixture to create mock drug data for testing."""
    ns = {"ns": "http://www.drugbank.ca"}

    # Drug 1: Has primary ID and multiple interactions
    drug1 = Element("{http://www.drugbank.ca}drug")
    drugbank_ids1 = SubElement(drug1, "{http://www.drugbank.ca}drugbank-id", {"primary": "true"})
    drugbank_ids1.text = "DB0001"
    interactions1 = SubElement(drug1, "{http://www.drugbank.ca}drug-interactions")
    interaction1_1 = SubElement(interactions1, "{http://www.drugbank.ca}drug-interaction")
    SubElement(interaction1_1, "{http://www.drugbank.ca}drugbank-id").text = "DB0002"
    SubElement(interaction1_1, "{http://www.drugbank.ca}description").text = "Interacts with DB0002."

    interaction1_2 = SubElement(interactions1, "{http://www.drugbank.ca}drug-interaction")
    SubElement(interaction1_2, "{http://www.drugbank.ca}drugbank-id").text = "DB0003"
    SubElement(interaction1_2, "{http://www.drugbank.ca}description").text = "Interacts with DB0003."

    # Drug 2: Has primary ID but no interactions
    drug2 = Element("{http://www.drugbank.ca}drug")
    drugbank_ids2 = SubElement(drug2, "d{http://www.drugbank.ca}rugbank-id", {"primary": "true"})
    drugbank_ids2.text = "DB0004"

    # Drug 3: No primary ID
    drug3 = Element("{http://www.drugbank.ca}drug")
    SubElement(drug3, "{http://www.drugbank.ca}drugbank-id").text = "DB0005"

    # Drug 4: Interaction with missing fields
    drug4 = Element("{http://www.drugbank.ca}drug")
    drugbank_ids4 = SubElement(drug4, "{http://www.drugbank.ca}drugbank-id", {"primary": "true"})
    drugbank_ids4.text = "DB0006"
    interactions4 = SubElement(drug4, "{http://www.drugbank.ca}drug-interactions")
    interaction4_1 = SubElement(interactions4, "{http://www.drugbank.ca}drug-interaction")
    SubElement(interaction4_1, "{http://www.drugbank.ca}drugbank-id")  # Missing text
    # Missing description element

    return [drug1, drug2, drug3, drug4], ns

"""
    Test the extraction of drug interactions data.

    Args:
        mock_interactions (list): A list of mock drug interactions.
"""
def test_extract_drug_interactions(mock_drugs_interactions):
    drugs, namespaces = mock_drugs_interactions

    # Call the function
    df = extract_drug_interactions(drugs, namespaces)

    # Expected data
    expected_data = [
        {"DrugBank ID (Primary)": "DB0001", "Interacting DrugBank ID": "DB0002", "Interaction Description": "Interacts with DB0002."},
        {"DrugBank ID (Primary)": "DB0001", "Interacting DrugBank ID": "DB0003", "Interaction Description": "Interacts with DB0003."},
        {"DrugBank ID (Primary)": "DB0006", "Interacting DrugBank ID": "", "Interaction Description": "No description"},
    ]

    expected_df = pd.DataFrame(expected_data)

    # Assertions
    pd.testing.assert_frame_equal(df, expected_df)

"""
    Parameterized test for extracting drug interactions data.

    This test checks the extraction of drug interactions from mock drug data 
    and compares the result to expected output. It verifies three cases:
    1. A drug with a single interaction.
    2. A drug with no interactions.
    3. A drug with a missing primary ID, which should result in no output.

    Args:
        drug_data (list): List of dictionaries containing drug interaction data.
        expected (list): Expected list of dictionaries in the resulting DataFrame.

    Asserts:
        Verifies that the function output matches the expected DataFrame.
"""
@pytest.mark.parametrize(
    "drug_data,expected",
    [
        # Test case 1: Single interaction
        (
            [{"primary_id": "DB0001", "interactions": [("DB0002", "Interacts with DB0002.")]}],
            [{"DrugBank ID (Primary)": "DB0001", "Interacting DrugBank ID": "DB0002", "Interaction Description": "Interacts with DB0002."}],
        ),
        # Test case 2: No interactions
        (
            [{"primary_id": "DB0001", "interactions": []}],
            [],
        ),
        # Test case 3: Missing primary ID
        (
            [{"primary_id": None, "interactions": [("DB0002", "Interacts with DB0002.")]}],
            [],
        ),
    ],
)
def test_extract_drug_interactions_parametrized(drug_data, expected):
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock drugs
    drugs = []
    for data in drug_data:
        drug = Element("{http://www.drugbank.ca}drug")
        if data["primary_id"]:
            db_id = SubElement(drug, "{http://www.drugbank.ca}drugbank-id", {"primary": "true"})
            db_id.text = data["primary_id"]
        interactions = SubElement(drug, "{http://www.drugbank.ca}drug-interactions")
        for interaction_id, interaction_desc in data["interactions"]:
            interaction = SubElement(interactions, "{http://www.drugbank.ca}drug-interaction")
            SubElement(interaction, "{http://www.drugbank.ca}drugbank-id").text = interaction_id
            SubElement(interaction, "{http://www.drugbank.ca}description").text = interaction_desc
        drugs.append(drug)

    # Call the function
    df = extract_drug_interactions(drugs, ns)

    # Create expected DataFrame
    expected_df = pd.DataFrame(expected)

    # Assertions
    pd.testing.assert_frame_equal(df, expected_df)

"""
    Test behavior when no interactions are found for a drug.

    Args:
        mock_interactions (list): A list of mock drug interactions.
"""
def test_extract_drug_interactions_no_interactions():
    ns = {"ns": "http://www.drugbank.ca"}

    # Create mock drugs with no interaction element
    drugs = []
    for i in range(3):
        drug = Element("{http://www.drugbank.ca}rug")
        db_id = SubElement(drug, "{http://www.drugbank.ca}drugbank-id", {"primary": "true"})
        db_id.text = f"DB000{i + 1}"
        drugs.append(drug)

    # Call the function
    df = extract_drug_interactions(drugs, ns)

    # Assertions
    assert df.empty