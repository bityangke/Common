"""
required packages
pip install pandas urllib3 tqdm numpy awscli
"""

import pandas as pd
import os
from tqdm import tqdm
from multiprocessing.dummy import Pool as ThreadPool


annotation_file = "D:/Projects/object_detection_data/test-annotations-bbox.csv"
class_names = ["/m/01940j"]
download_dir = "D:/Projects/object_detection_data/"

# this is where you select where the data is downloaded from
data_type = "train"
# data_type = "validation"
# data_type = "test"

# read in the file
f = pd.read_csv(annotation_file)

# parse the file and then pull out the lines that match the class name
u = f.loc[f['LabelName'].isin(class_names)]

threads = 10
pool = ThreadPool(threads)

commands = []

# cycle through each entry and add to the commands list
for idx in u.index:
    image_filename = u['ImageID'][idx] + ".jpg"
    command = "aws s3 --no-sign-request --only-show-errors cp s3://open-images-dataset/" + data_type + "/" + image_filename + " ."
    # print(command)
    commands.append(command)

# start downloading the images
list(tqdm(pool.imap(os.system, commands), total = len(commands) ))

print('Done!')
pool.close()
pool.join()

