import os
import sys
import math
import rospy
from sensor_msgs.msg import Image
from std_msgs.msg import String
from cv_bridge import CvBridge
import cv2
import numpy as np
import tensorflow as tf

from collections import defaultdict
from io import StringIO

sys.path.append("..")

# these are from the tensorflow/models/research/object_detection/
from object_detection.utils import ops as utils_ops
from utils import label_map_util
from utils import visualization_utils as vis_util

min_score = 0.5

MODEL_PATH = '/home/ros/'

# MSCOCO
MODEL_NAME = 'ssd_resnet50_v1_fpn'
PATH_TO_LABELS = os.path.join('/home/ros/models/research/object_detection/data', 'mscoco_label_map.pbtxt')
NUM_CLASSES = 90

#Open Image
#MODEL_NAME = 'ssd_mobilenet_v2_oid'
#PATH_TO_LABELS = os.path.join('/home/ros/models/research/object_detection/data', 'oid_v4_label_map.pbtxt')
#NUM_CLASSES = 600

#Open Image
#MODEL_NAME = 'faster_rcnn_inception_resnet_v2_oid'
#PATH_TO_LABELS = os.path.join('/home/ros/models/research/object_detection/data', 'oid_v4_label_map.pbtxt')
#NUM_CLASSES = 600

PATH_TO_CKPT = MODEL_PATH + MODEL_NAME + '/frozen_inference_graph.pb'

class RosTensorFlow():
    def __init__(self):
        self.label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
        self.categories = label_map_util.convert_label_map_to_categories(self.label_map, max_num_classes=NUM_CLASSES, use_display_name=True)
        self.category_index = label_map_util.create_category_index(self.categories)
        
        self._cv_bridge = CvBridge()

        self.load_graph()
        self._session = tf.Session(graph=self.detection_graph)        

        self._sub = rospy.Subscriber('image', Image, self.callback, queue_size=1)
        self._img_pub = rospy.Publisher('obj_det/image', Image, queue_size=1)
        self._box_pub = rospy.Publisher('obj_det/boxes', String, queue_size=1)
        #self.score_threshold = rospy.get_param('~score_threshold', 0.1)
        #self.use_top_k = rospy.get_param('~use_top_k', 5)

    def callback(self, image_msg):
        cv_image = self._cv_bridge.imgmsg_to_cv2(image_msg, "rgb8")
        img_height = cv_image.shape[0]
        img_width  = cv_image.shape[1]
                
        # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
        image_np_expanded = np.expand_dims(cv_image, axis=0)
        image_tensor = self.detection_graph.get_tensor_by_name('image_tensor:0')

        # Each box represents a part of the image where a particular object was detected.
        boxes = self.detection_graph.get_tensor_by_name('detection_boxes:0')

        # Each score represent how level of confidence for each of the objects.
        # Score is shown on the result image, together with the class label.
        scores = self.detection_graph.get_tensor_by_name('detection_scores:0')
        classes = self.detection_graph.get_tensor_by_name('detection_classes:0')
        num_detections = self.detection_graph.get_tensor_by_name('num_detections:0')

        # Actual detection.
        (boxes, scores, classes, num_detections) = self._session.run(
            [boxes, scores, classes, num_detections],
            feed_dict={image_tensor: image_np_expanded})
        
        boxes = np.squeeze(boxes)
        classes = np.squeeze(classes).astype(np.int32)
        scores = np.squeeze(scores)

        box_string = ""
        for idx in range(num_detections):
            if scores[idx] >= min_score:
                box_string = box_string + "{Class=" + self.category_index[classes[idx]]['name'] + "; xmin={}, ymin={}, xmax={}, ymax={}".format(math.floor(boxes[idx][1]*img_width), math.floor(boxes[idx][0]*img_height), math.ceil(boxes[idx][3]*img_width), math.ceil(boxes[idx][2]*img_height)) + "}, "

        box_string = box_string[:-2]        
        
        # Visualization of the results of a detection.
        vis_util.visualize_boxes_and_labels_on_image_array(
            cv_image,
            (boxes),
            (classes),
            (scores),
            self.category_index,
            use_normalized_coordinates=True,
            min_score_thresh=min_score,
            line_thickness=8)        
        
        self._img_pub.publish(self._cv_bridge.cv2_to_imgmsg(cv_image, "rgb8"))
        self._box_pub.publish(box_string)
      
                
    ## load in the detection graph from the frozen checkpoint file    
    def load_graph(self):
        self.detection_graph = tf.Graph()
        with self.detection_graph.as_default():
          od_graph_def = tf.GraphDef()
          with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
            serialized_graph = fid.read()
            od_graph_def.ParseFromString(serialized_graph)
            tf.import_graph_def(od_graph_def, name='')
    
    def main(self):
        rospy.spin()


if __name__ == '__main__':
    #classify_image.setup_args()
    rospy.init_node('rostensorflow')
    tensor = RosTensorFlow()
    tensor.main()
    

