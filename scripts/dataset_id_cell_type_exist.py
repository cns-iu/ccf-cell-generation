import csv
import os
import requests


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
    print(files)
    for file in files:
        if file.startswith('HBM'):
            url = 'https://entity.api.hubmapconsortium.org/entities/' + file[:-4]
            response = requests.get(url).json()
            if 'uuid' in response:
                print(url)
                print(response['entity_type'])



    # with open('./csv/watertight_mesh_dataset_counter.csv', 'r', encoding='UTF-8') as f1, \
    #         open('./csv/tissue_rui_with_dataset_id_exist.csv', 'w', encoding='UTF-8', newline='') as f2:
    #     reader = csv.reader(f1, delimiter=',')
    #     writer = csv.writer(f2, delimiter=',')
    #
    #     writer.writerow(['Anatomical Structures', 'Organ', 'is_watertight (T/F)',
    #                      'number_of_tissue_blocks_registered_total_Lu_collision',
    #                      'number_of_datasets_with_ct_counts_for this_anatomical_structure',
    #                      'dataset_ids', 'number_of_existing_dataset_ids', 'existing_dataset_ids'])
    #
    #     for row in reader:
    #         existing_csv = []
    #         dataset_ids = row[5].split()
    #         for dataset_id in dataset_ids:
    #             if dataset_id + '.csv' in files:
    #                 existing_csv.append(dataset_id)
    #
    #         writer.writerow([row[0], row[1], row[2], row[3], row[4], row[5], len(existing_csv), ",".join(existing_csv)])









