import collections
import urllib.request
import json
import requests
import csv
import os


url_tissue_rui = "https://ccf-api.hubmapconsortium.org/v1/hubmap/rui_locations.jsonld"
url_reference_organ_data = "https://hubmapconsortium.github.io/ccf-ui/rui/assets/reference-organ-data.json"
url_collision_detection = "http://192.168.1.100:8080/get-collisions"

mapping_rui_uuid = {}
mapping_uuid_hubmap_id = {}

mapping_rui_hubmap_id = {}


def extract_csv_files(path):

    files = set()

    for folder in os.listdir(path):
        full_folder = os.path.join(path, folder)
        if os.path.isdir(full_folder):
            for file_name in os.listdir(full_folder):
                files.add(file_name)

    return files


if __name__ == "__main__":
    files = extract_csv_files('../../tissue-bar-graphs/csv')

    with urllib.request.urlopen(url_tissue_rui) as content:
        data = json.loads(content.read().decode())
        graph = data['@graph']

        for person in graph:
            samples = person['samples']

            for sample in samples:

                if sample['@type'] == 'Sample':

                    total_uuid = set()
                    tissue_block_entity_id = sample['@id']
                    # response = requests.get(tissue_block_entity_id)
                    # sample_hubmap_id = response['direct_ancestor']['hubmap_id']
                    print(tissue_block_entity_id)
                    uuid = tissue_block_entity_id.split('/')[-1]
                    total_uuid.add(uuid)

                    sections = sample['sections']
                    for section in sections:
                        sd_entity_id = section['@id']
                        uuid = sd_entity_id.split('/')[-1]
                        section_datasets = section['datasets']

                        for sd in section_datasets:
                            sd_dataset_entity_id = sd['@id']
                            uuid = sd_dataset_entity_id.split('/')[-1]
                            total_uuid.add(uuid)

                    datasets = sample['datasets']
                    for dataset in datasets:
                        dataset_entity_id = dataset['@id']
                        uuid = dataset_entity_id.split('/')[-1]
                        total_uuid.add(uuid)

                    mapping_rui_uuid[tissue_block_entity_id] = list(total_uuid)

    print(mapping_rui_uuid)

    for file in files:
        if file.startswith('HBM'):
            url = 'https://entity.api.hubmapconsortium.org/entities/' + file[:-4]
            response = requests.get(url).json()
            if 'uuid' in response:
                uuid = response['uuid']
                hubmap_id = response['hubmap_id']
                mapping_uuid_hubmap_id[uuid] = hubmap_id

    with open('./csv/mapping_rui_uuid_hubmap_id.csv', 'w', encoding='UTF-8', newline='') as f:

        writer = csv.writer(f)
        writer.writerow(['hubmap_id', 'uuid', 'rui'])
        for rui, uuids in mapping_rui_uuid.items():
            total_hubmap_id = []

            for uuid in uuids:
                if uuid in mapping_uuid_hubmap_id:
                    hubmap_id = mapping_uuid_hubmap_id[uuid]
                    total_hubmap_id.append(hubmap_id)
                    writer.writerow([hubmap_id, uuid, rui])

            mapping_rui_hubmap_id[rui] = total_hubmap_id

    print(mapping_rui_hubmap_id)








