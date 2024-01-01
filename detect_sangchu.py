# -*- coding: utf-8 -*-
import cv2
import numpy as np
import time
import Jetson.GPIO as GPIO
import smbus

bus = smbus.SMBus(1)

# 아두이노의 I2C 주소
arduino_address = 0x12

GPIO.setmode(GPIO.BCM)

# Load Yolo
net = cv2.dnn.readNet("yolo-obj_30000.weights", "yolo-obj.cfg")

net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA)

classes = []

with open("obj.names", "r") as f:
    classes = [line.strip() for line in f.readlines()]
layer_names = net.getLayerNames()
output_layers = [layer_names[i - 1] for i in net.getUnconnectedOutLayers()]
colors = np.random.uniform(0, 255, size=(len(classes), 3))

def main(video_path):
    cap = cv2.VideoCapture(video_path)
    cnt = 0
    frame = 0
    normal_score = 0
    start_time = time.time()
    capture_time = start_time

    while cap.isOpened():
        ret, image = cap.read()
        if not ret:
            break
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        height, width, channels = image.shape

        # Detecting objects (408 408)
        blob = cv2.dnn.blobFromImage(image, 0.00392, (408, 408), (0, 0, 0), True, crop=False)

        net.setInput(blob)
        outs = net.forward(output_layers)
        
        # Showing informations on the screen
        class_ids = []
        confidences = []
        boxes = []
        for out in outs:
            for detection in out:
                scores = detection[5:]
                class_id = np.argmax(scores)
                confidence = scores[class_id]
                if confidence > 0.3:
                    # Object detected
                    center_x = int(detection[0] * width)
                    center_y = int(detection[1] * height)
                    w = int(detection[2] * width)
                    h = int(detection[3] * height)

                    # Rectangle coordinates
                    x = int(center_x - w / 2)

                    y = int(center_y - h / 2)

                    boxes.append([x, y, w, h])
                    confidences.append(float(confidence))
                    class_ids.append(class_id)

        indexes = cv2.dnn.NMSBoxes(boxes, confidences, 0.3, 0.4)
        font = cv2.FONT_HERSHEY_PLAIN

        labels = []

        for i in range(len(boxes)):
            if i in indexes:
                x, y, w, h = boxes[i]
                label = str(classes[class_ids[i]])
                labels.append(label)
                #color = colors[class_ids[i]]
                #cv2.rectangle(image, (x, y), (x + w, y + h), color, 2)
                #cv2.putText(image, label, (x, y + 30), font, 3, color, 3)
        print(labels)
        if 'disease' in labels:
            disease_score += 1

            if disease_score > 3:
                print('detect normal sangchu')
                # 데이터 송신
                data = [0x01]  # 전송할 데이터 리스트
                bus.write_i2c_block_data(arduino_address, 0x00, data)
                print("Data sent to Arduino:", data)
                disease_score=0
                time.sleep(3)  # 3초 동안 대기
                # time delay시간은 아두이노에서 모터가 멈추고 물을 쏜 다음 가동 시작할 때 까지의 시간을 딜레이주면

        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
        cv2.imshow('Image', image)
        frame += 1
        
        #if time.time() - capture_time >= 1:
            # 캡처된 프레임 저장
        #    file_path = os.path.join(capture_folder, "j3_sangchu_capture_{}_{}.jpg".format(cnt, frame))
        #   img_captured = cv2.imwrite(file_path, image)
        #    cnt += 1
        #    capture_time = time.time()

        if cv2.waitKey(1) == ord('q'):
            end_time = time.time()
            print('running time : ', end_time - start_time)
            print('Frame : ', frame)
            break

    cv2.destroyAllWindows()

if __name__ == '__main__':
    # video_path = 0
    # video_path = 'sang.mp4'
#    video_path = 'sangchu5.mp4'
    video_path = 'sangchu8.mp4'
    # main(video_path)

