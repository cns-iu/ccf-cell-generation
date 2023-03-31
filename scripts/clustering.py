import json
import os
import collections
import requests
import csv
import pandas

url_collision_detection = "http://192.168.1.100:8080/get-collisions"
# only consider the HuBMAP Kidney "C:/Users/catherine/Desktop/research/hubmap/tissue-bar-graphs/csv/Kidney_HuBMAP_Portal"
def hubmap_kidney_dataset(csv_dir_path):
    dir_list = os.listdir(csv_dir_path)
    return [e[:-4] for e in dir_list if e.endswith('.csv')]


# dic[dataset_id, tissue_block_rui_location containing tissue_block_id]
def get_rui_locations(json_path):
    dic = {}
    with open(json_path, 'r') as f:
        json_data = json.load(f)
        for dataset_id, values in json_data.items():
            if 'rui-location' in values:

                rui_location = values['rui-location']
                if not rui_location:
                    continue

                tissue_block_id = values['hubmap-id']
                rui_location["tissue_block_id"] = tissue_block_id
                dic[dataset_id] = rui_location
                dic[tissue_block_id] = rui_location

    return dic


def visualize(dataset_path, json_path):
    datasets = hubmap_kidney_dataset(dataset_path)
    dic = get_rui_locations(json_path)

    for dataset_id in datasets:
        if dataset_id not in dic:
            continue
        rui_location = dic[dataset_id]
        # save the tissue blocks in OFF format by sending the post request
        requests.post(url=url_collision_detection, json=rui_location)


def save_rui_locations(json_path):
    pass


if __name__ == "__main__":
    dataset_path = "C:/Users/catherine/Desktop/research/hubmap/tissue-bar-graphs/csv/Kidney_HuBMAP_Portal"
    json_path = "ct_pop_total.json"

    visualize(dataset_path, json_path)







