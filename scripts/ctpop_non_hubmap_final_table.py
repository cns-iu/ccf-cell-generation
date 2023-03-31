import pandas as pd
import os
import json


def load_tissue_block(json_file_path):
    is_exist_dic = {}
    with open(json_file_path) as f:
        json_data = json.load(f)

        for dataset_id, values in json_data.items():
            if "rui_location" in values:
                rui_location = values.get("rui_location")
                if rui_location:
                    # rui_dic[dataset_id] = rui_location
                    # rui_dic[dataset_id + '.csv'] = rui_location
                    is_exist_dic[dataset_id] = 'Yes'
                    is_exist_dic[dataset_id + '.csv'] = 'Yes'

    return is_exist_dic


def map_dataset_id(json_file_path, training_prediction_table_path, output_file_path):

    is_exist_dic = load_tissue_block(json_file_path)
    df = pd.read_csv(training_prediction_table_path, header=1)
    df['is_exist_rui'] = df['dataset_id'].map(is_exist_dic)
    df2 = df[['dataset_id', 'is_exist_rui']]
    df2.to_csv(output_file_path, index=False)


if __name__ == "__main__":
    json_file_path = "non-hubmap-rui-location.json"
    training_prediction_table_path = "CTPop - Training_Prediction Datasets - Copy of Training_Prediction.csv"
    output_file_path = "non_hubmap_exist.csv"
    map_dataset_id(json_file_path, training_prediction_table_path, output_file_path)




