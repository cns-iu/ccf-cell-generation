import json
import pandas as pd
import requests
import urllib.request


# dataset_id here is dataset_id + donor_id + organ for CxG data. all keys are without .csv
def load_tissue_block(rui_dic, json_file_path):
    with open(json_file_path) as f:
        json_data = json.load(f)

        for dataset_id, values in json_data.items():
            if "rui-location" in values:
                rui_location = values.get("rui-location")
                if rui_location:
                    if dataset_id.endswith(".csv"):
                        rui_dic[dataset_id[:-4]] = rui_location
                    else:
                        rui_dic[dataset_id] = rui_location


def post_request_collision_detection(row):
    rui_location = row['rui_location']
    if pd.notna(rui_location):
        url_collision_detection = 'http://192.168.1.100:8080/get-collisions'
        collision_response = requests.post(url=url_collision_detection, json=rui_location)
        collision_response = collision_response.json()
        list_of_colliding_AS = [AS_item['node_name'] for AS_item in collision_response]
        uberon_ids = set([AS_item["representation_of"] for AS_item in collision_response])
        uberon_ids.discard('-')

        row['list_of_colliding_AS'] = "|".join(list_of_colliding_AS)
        row['rui_location_annotations_Lu'] = "|".join(uberon_ids)

    return row


def get_annotation(rui_location):
    if pd.notna(rui_location):
        ccf_annotations = rui_location["ccf_annotations"]
        return "|".join(ccf_annotations)


def get_organ(rui_location):

    if pd.notna(rui_location):
        # url_collision_detection = 'http://192.168.1.100:8080/get-collisions'
        # collision_response = requests.post(url=url_collision_detection, json=rui_location)
        # collision_response = collision_response.json()
        # if len(collision_response) > 0:
        #     organ = collision_response[0]['organ']
        #     return organ
        target = rui_location['placement']['target']

        return target.split('#')[-1]


def get_dataset_id_for_rui(row):
    print(row)
    source = row['source']

    if source == 'CxG':
        dataset_id = row['CxG_dataset_id_donor_id_organ']
    else:
        dataset_id = row['dataset_id'][:-4] if row['dataset_id'].endswith('.csv') else row['dataset_id']

    return dataset_id


def filling_training_prediction(rui_dic, training_prediction_table_path):
    df = pd.read_csv(training_prediction_table_path)
    df['dataset_id_for_rui'] = df.apply(get_dataset_id_for_rui, axis=1)
    df['rui_location'] = df['dataset_id_for_rui'].map(rui_dic)
    df['organ'] = df['rui_location'].apply(get_organ)
    df['rui_location_annotations_json'] = df['rui_location'].apply(get_annotation)
    # df['list_of_colliding_AS'] = df['rui_location'].apply(post_request_collision_detection)
    df = df.apply(post_request_collision_detection, axis=1)
    df2 = df[['dataset_id', 'dataset_id_for_rui', 'organ', 'rui_location_annotations_json',
              'rui_location_annotations_Lu', 'list_of_colliding_AS']]

    return df2


def compute_tissue_block_volume(json_file_path, training_prediction_table_path, output_file_path):
    volume_dic = {}
    rui_dic = {}
    load_tissue_block(rui_dic, json_file_path)

    df = filling_training_prediction(rui_dic, training_prediction_table_path)
    df.to_csv(output_file_path, index=False)


if __name__ == "__main__":
    json_file = "all_rui_location_by_dataset_id.json"
    output_file = "./csv/tissue_block_collision_detection.csv"

    # points to the training prediction table
    training_prediction_file = "CTPop - Training_Prediction Datasets - Training_Prediction.csv"
    compute_tissue_block_volume(json_file, training_prediction_file, output_file)






