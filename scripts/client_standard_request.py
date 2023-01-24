import collections
import urllib.request
import json
import requests
import csv

table = []
url_tissue_rui = "https://ccf-api.hubmapconsortium.org/v1/hubmap/rui_locations.jsonld"
url_reference_organ_data = "https://hubmapconsortium.github.io/ccf-ui/rui/assets/reference-organ-data.json"
url_collision_detection = "http://192.168.1.100:8080/get-collisions"
maxWidth = 0

scaling_transformation = {}

responses = []
counter = collections.Counter()
dataset_id_per_AS = collections.defaultdict(list)

with urllib.request.urlopen(url_reference_organ_data) as content:
    data = json.loads(content.read().decode())
    print(data.keys())
    placement = data['placementPatches']

    for _, mapping in placement.items():
        source = mapping["source"]
        target = mapping["target"]
        x_scaling = mapping["x_scaling"]
        y_scaling = mapping["y_scaling"]
        z_scaling = mapping["z_scaling"]
        target_scaling = {"target": target, "x_scaling": x_scaling, "y_scaling": y_scaling, "z_scaling": z_scaling}
        scaling_transformation[source] = target_scaling


with urllib.request.urlopen(url_tissue_rui) as content:
    data = json.loads(content.read().decode())
    graph = data['@graph']

    for person in graph:
        samples = person['samples']
        for sample in samples:
            if sample['sample_type'] == 'Tissue Block':
                rui_location = sample['rui_location']
                placement = rui_location['placement']

                target = placement['target']
                organ = target

                if organ in scaling_transformation:
                    mapping = scaling_transformation[organ]
                    organ = mapping["target"]
                    placement['target'] = organ
                    # print("old organ after mapping:{}", organ)

                    placement["x_translation"] *= mapping["x_scaling"]
                    placement["y_translation"] *= mapping["y_scaling"]
                    placement["z_translation"] *= mapping["z_scaling"]

                r = requests.post(url=url_collision_detection, json=rui_location)
                if r:
                    try:
                        print(rui_location)
                        print(r.content)
                        print(r.json())

                        responses.append([sample['@id'], r.json()])

                    except:
                        pass
                    # response = {'@id': rui_location['@id'], 'result': r.json()}
                    # responses.append(response)

# final result
mapping_rui_dataset_id = collections.defaultdict(list)

with open('./csv/mapping_rui_uuid_hubmap_id.csv', 'r', encoding='UTF-8') as f:
    csv_reader = csv.reader(f, delimiter=',')
    for row in csv_reader:
        hubmap_id, uuid, rui = row
        mapping_rui_dataset_id[rui].append(hubmap_id)

for tissue_id, response in responses:
    for item in response:
        organ = item['organ']
        node_name = item['node_name']
        counter[(organ, node_name)] += 1
        dataset_id_per_AS[(organ, node_name)].extend(mapping_rui_dataset_id[tissue_id])


with open('watertight_results.csv', 'r', encoding='UTF-8') as f1, open('./csv/watertight_mesh_dataset_counter.csv', 'w', encoding='UTF-8', newline='') as f2:
    reader = csv.reader(f1, delimiter=',')
    writer = csv.writer(f2, delimiter=',')
    writer.writerow(['Anatomical Structures',
                     'Organ',
                     'is_watertight (T/F)',
                     'number_of_tissue_blocks_registered_total_Lu_collision',
                     'number_of_datasets_with_ct_counts_for this_anatomical_structure',
                     'dataset_ids'])

    line_count = 0
    for row in reader:
        if line_count > 0:
            organ, mesh, manifold, closed, self_intersection, watertight = row
            writer.writerow([mesh, organ, watertight, counter[(organ, mesh)], len(dataset_id_per_AS[(organ, mesh)]),
                             ",".join(dataset_id_per_AS[(organ, mesh)])
                             ])
        line_count += 1


# with open('watertight_mesh_counter.csv', 'w', encoding='UTF-8', newline='') as f:
#     writer = csv.writer(f)
#
#     pass

#                 row = [rui_location['@id']]
#                 width = 0
#                 percentage_sum = 0
#                 for element in r.json():
#                     row.append(element['node_name'])
#                     row.append(element['label'])
#                     row.append(element['representation_of'])
#                     row.append(element['percentage'])
#                     row.append(element['volume'])
#                     width += 5
#                     percentage_sum += element['percentage']
#
#                 maxWidth = max(maxWidth, width)
#                 row.insert(1, percentage_sum)
#                 print(row)
#                 table.append(row)
#
# print("max width of rows: {}".format(maxWidth))
#
# max_collision_AS = maxWidth // 5
#
# header = ['tissue block id', 'total percentage']
#
# for i in range(1, max_collision_AS + 1):
#     header.append('node_name' + '/' + str(i))
#     header.append('ccf_annotation' + '/' + str(i))
#     header.append('representation_of' + '/' + str(i))
#     header.append('percentage' + '/' + str(i))
#     header.append('volume' + '/' + str(i))
#
# with open('VHMRightKidney_tabular.csv', 'w', encoding='UTF8', newline='') as f:
#     writer = csv.writer(f)
#
#     # write the header
#     writer.writerow(header)
#
#     # write multiple rows
#     writer.writerows(table)