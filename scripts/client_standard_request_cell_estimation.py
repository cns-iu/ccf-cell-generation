import urllib.request
import json
import requests
import csv
import os

url_collision_detection = "http://192.168.1.100:8080/get-collisions"

x_scaling = 0.94
y_scaling = 1.097
z_scaling = 0.94


def handle_request(file_name):
    f = open(file_name, "r")
    rui_location = json.load(f)
    placement = rui_location['placement']
    target = placement['target']

    if target.endswith("VHFRightKidney") or target.endswith("VHFLeftKidney"):
        placement["x_translation"] = placement["x_translation"] * x_scaling
        placement["y_translation"] = placement["y_translation"] * y_scaling
        placement["z_translation"] = placement["z_translation"] * z_scaling

    response = requests.post(url=url_collision_detection, json=rui_location)
    response = response.json()

    return response


if __name__ == "__main__":

    folder = "./tissue_rui_for_cell_estimation"

    for f in os.listdir(folder):
        tissue_path = os.path.join(folder, f)
        response = handle_request(tissue_path)
        print(f)
        print(response)
    # response = handle_request("./tissue_rui_for_cell_estimation/HBM926.GQBN.564.json")
    # print(response)